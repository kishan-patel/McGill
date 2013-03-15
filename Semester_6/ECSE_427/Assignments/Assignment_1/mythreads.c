#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <slack/std.h>
#include <slack/list.h>
#include "mythreads.h"

//Set to 1 to enable print statements to be printed for debuggin purposes.
#define DEBUG 0

/*Holds information about each thread (i.e. the name, id, state, run_time...).*/
static ControlBlock tcbTable[THREAD_MAX];
/*Holds information about each semaphore (i.e. value, waitqueue...).*/
static Semaphore semTable[SEMAPHORE_MAX];
/*Contains the threads that are waiting to run.*/
static List * runqueue;
/*Id of the thread.*/
static int threadId = 0;
/*Id of the currently running thread.*/
static int runningThreadId = 0;
/*Points to the main context.*/
static ucontext_t uctx_main;
/*Period at which thread preemption will occur.*/
static int quantum_size;
/*Time to schedule ALRM signal interrup.*/
struct itimerval timer;
/*The value of the current_semaphore.*/
static int current_semaphores;
/*Flag to signal if we're done.*/
static int done = 0;
/*Count which keeps a track of the total threads created. At the end of program run totalThreadsCreated 
should equal total totalThreadsExited.*/
static int totalThreadsCreated = 0;
/*Count to keep a track of the total threads exited. At the end of program run totalThreadsCreated 
should equal total totalThreadsExited.*/
static int totalThreadsExited = 0;
/*Count to keep a track of the number of switches that have taken place.*/
int noSwitches;
/*For loop variable.*/
static int i=0;

/*
This function initializes all the globa data structures for the thread system
*/
int mythread_init(){
  //Initialize the run queue.
  runqueue = list_create(NULL);
  
  //Initialize the thread control table.
  for(i = 0;i < THREAD_MAX;i++){
    tcbTable[i].state = NOTCREATED;
  }
  
  //Initialize the semaphore table.
  for(i = 0;i < SEMAPHORE_MAX;i++){
    semTable[i].active = 0;
  }
}

/*
This function creates a new thread and returns an integer that points to the thread
control block that is allocated to the newly created thread in the thread control
block table. It returns -1 if it is unable to create a new thread. Further, 
the function creates and initializes the user context for the thread being created.
*/
int mythread_create(char *threadName, void(*threadfunc)(), int stacksize){
  //Set basic information about the thread.
  strcpy(tcbTable[threadId].thread_name, threadName);
  tcbTable[threadId].thread_id = threadId;
  tcbTable[threadId].state = RUNNABLE;
 
  //Set the context information about the thread.
  getcontext(&tcbTable[threadId].context); //Save the current context.
  tcbTable[threadId].context.uc_link = &uctx_main; //The context that will be resumed when the current context exits.
  tcbTable[threadId].context.uc_stack.ss_sp = malloc(stacksize); //Allocate the stack and make it point to the beginning of the stack.
  tcbTable[threadId].context.uc_stack.ss_size = stacksize; //Set the signal stack size.
  makecontext(&tcbTable[threadId].context,threadfunc,0); //Creates the context with the information set above.
  
  //Add thread to the runqueue.
  runqueue = list_append_int(runqueue,threadId);
  totalThreadsCreated++;
  
  //If an error occured, return -1. Otherwise, return the  thread id.
  if(!tcbTable[threadId].context.uc_stack.ss_sp){
    //We failed to allocate memory for the stack
    #if DEBUG == 1
      printf("Failed to allocate memory for the stack\n");
      fflush(stdout);
    #endif
    
    return -1;
  }else{
    #if DEBUG == 1
      printf("state of thread %s is %d\n",tcbTable[threadId].thread_name,tcbTable[threadId].state);
    #endif
    
    int tmp = threadId;
    threadId++;
    return tmp;
  }
}

/*
This function is called at the end of the function that was 
invoked by the thread (i.e. the handler() function in my-test.c).
*/
void mythread_exit(){
  //Block signals
  sigset_t x;
  sigemptyset(&x);
  sigaddset(&x, SIGALRM);
  sigprocmask(SIG_BLOCK, &x, NULL);

  #if DEBUG == 1
    printf("Setting state of thread %d to EXIT.\n",runningThreadId);
    fflush(stdout);
  #endif
  
  //Set the state of the thread to EXIT.
  tcbTable[runningThreadId].state = EXIT; 
  totalThreadsExited++;
  if(totalThreadsCreated == totalThreadsExited){
    done = 1;
  }
  sigprocmask(SIG_UNBLOCK, &x, NULL);
}

/*
Starts running one of the threads in the runqueue. When this method exits,
all of the created threads should have completed running.
*/
void runthreads(){
  //Block Signals
  sigset_t x;
  sigemptyset(&x);
  sigaddset(&x, SIGALRM);
  sigprocmask(SIG_BLOCK, &x, NULL);

  //Create a timer to know when the scheduler should switch contexts.
  struct itimerval timer;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = quantum_size;
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = quantum_size;
  setitimer(ITIMER_REAL, &timer, 0);
  sigset(SIGALRM, &scheduler);
  
  //Get the first thread that should run.
  runningThreadId = list_shift_int(runqueue);
  tcbTable[runningThreadId].state = RUNNING;
  clock_gettime(CLOCK_REALTIME,&tcbTable[runningThreadId].start);
  
  //Unblock signal 
  sigprocmask(SIG_UNBLOCK, &x, NULL);

  //Go to the context for the thread (i.e. handler() function). Also, save the 
  //current context in uctx_main.
  if(swapcontext(&uctx_main, &tcbTable[runningThreadId].context) == -1) {
      perror("swapcontext error");
      exit(1);
  }

  //To ensure that there are still no remaining threads left.
  while(totalThreadsExited<totalThreadsCreated);
}

/*
  This function sets the quantum size of the round robin scheduler. This is 
  the period when context switches will occur.
*/
void set_quantum_size(int size){
  if(size < MIN_QUANTUM_SIZE)
    quantum_size = MIN_QUANTUM_SIZE;
  else  
    quantum_size = size;
}

/*
  The scheduler is called each time an ALRM signal is fired. It is responsible
  for preempting the currently running thread and giving a chance to another
  thread that is eligible to run in the runqueue.
*/
void scheduler(){
  noSwitches++;
  if (!list_empty(runqueue) ) {
    //Get the next thread to run and save the current one in a temp variable.
    int threadToPreempt = runningThreadId;
    runningThreadId = list_shift_int(runqueue);
    
    //Update the times to print the total time run on the CPU at the end of 
    //the program.
    clock_gettime(CLOCK_REALTIME, &tcbTable[threadToPreempt].end);
    tcbTable[threadToPreempt].run_time += tcbTable[threadToPreempt].end.tv_nsec - tcbTable[threadToPreempt].start.tv_nsec;
    clock_gettime(CLOCK_REALTIME, &tcbTable[runningThreadId].start);
    
    if (tcbTable[threadToPreempt].state == RUNNABLE || tcbTable[threadToPreempt].state == RUNNING) {
        //If the thread we are about to preempt is still RUNNABLE or RUNNING, we 
        //add it to the runqueue so that it can be scheduler at a later time.
        runqueue = list_append_int(runqueue, threadToPreempt);
    }
    
    //Perform the context switch.
    if (swapcontext(&tcbTable[threadToPreempt].context, &tcbTable[runningThreadId].context) == -1) {
        printf("swapcontext error\n");
        fflush(stdout);
    }
  }
}

/*
  This function creates a sempahore and sets its initial value to the given 
  parameter. Each entry of this table will be a structure that defines the complete
  state of the semaphore.
*/
int create_semaphore(int val){
  if (current_semaphores == SEMAPHORE_MAX){
    //We already have the maximum number of semphores allowed 
    //so we cannot create new ones.
    return -1;
  }else{
    //Create the semaphore with the appropriate values. And create the 
    //a wait queue for this semaphore as well.
    semTable[current_semaphores].initial = val;
    semTable[current_semaphores].value = val;
    semTable[current_semaphores].thread_queue = list_create(NULL);
    int tmp = current_semaphores;
    current_semaphores++;
    return tmp;
  }
}

/*
  When a thread calls this function, the value of the sempahore is decremented. If the 
  value goes below 0, the thread is put into a BLOCKED state. The calling thread
  is no longer in the runqueue and will be inserted into the waitqueue for the 
  given sempahore.
*/
void semaphore_wait(int semaphore){
  sigset_t x;
  sigemptyset(&x);
  sigaddset(&x, SIGALRM);
  sigprocmask(SIG_BLOCK, &x, NULL);

  int tmpNoSwitches = noSwitches; 
  //Decrement the semaphore value.
  (semTable[semaphore]).value--;

  if((semTable[semaphore]).value<0) {
      //This thread has to wait so we set it's state to block and put it in the 
      //wait queue for the given semaphore.
      (tcbTable[runningThreadId]).state = BLOCKED;
      (semTable[semaphore]).thread_queue = list_append_int((semTable[semaphore]).thread_queue,runningThreadId);
  }

  sigprocmask(SIG_UNBLOCK,&x,NULL);
  while(tmpNoSwitches==noSwitches);
}

/*
  When a thread calls this function, the value of the semaphore is incremented. If the 
  value is less than one, then we should have at least one thread waiting on it. If
  this is the case then we take the thread waiting on the current semaphore out
  of the semaphore wait queue and insert it into the runqueue.
*/
void semaphore_signal(int semaphore){
  // Block Signals
  sigset_t x;
  sigemptyset (&x);
  sigaddset(&x, SIGALRM);
  sigprocmask(SIG_BLOCK, &x, NULL);
  
  /* Increase Semaphore value */
  semTable[semaphore].value = semTable[semaphore].value + 1;
  
  if (semTable[semaphore].value < 1) {
      int waitingThread;
      //Get the thread that has been granted the semphore.
      waitingThread = list_shift_int(semTable[semaphore].thread_queue);
      //Set the state of the thread granted the semaphore to runnable.
      tcbTable[waitingThread].state = RUNNABLE;
      //Add this thread to the back of the runqueue.
      runqueue = list_append_int(runqueue, waitingThread);
  } 

  // Unblock Signals
  sigprocmask(SIG_UNBLOCK, &x, NULL);
  scheduler();
}

/*
  This function removes a sempahore from the system. There are two cases where it
  could fail. Firstly, if there are still threads waiting on the semaphore. Secondly,
  if the current value of the semaphore is not equal to the initial value.
*/
void destroy_semaphore(int semaphore){
  if(!list_empty(semTable[semaphore].thread_queue)){
    fprintf(stderr, "There are threads waiting on this semaphore. Thus, it cannot be destroyed\n");
  }
  
  if(semTable[semaphore].initial != semTable[semaphore].value){
    fprintf(stderr,"The intial value of the semaphore doesn't match the one you're trying to delete\n");
  }
  
  return;
}

/*
  Prints statistics.
*/
void mythread_state()
{
    printf("Thread Name\tThread Id\tThread State\tTime Run on CPU (ns)\n");
    fflush(stdout);
    printf("-------------------------------------------------------------------\n");
    fflush(stdout);
    for(i = 0; (tcbTable[i].state != NOTCREATED) && (i < THREAD_MAX); ++i) {
        char * stateOfThread;
        if(tcbTable[i].state == 0)
        {
          stateOfThread = "NOTCREATED";
        }
        else if(tcbTable[i].state == 1)
        {
          stateOfThread = "RUNNING";
        }
        else if(tcbTable[i].state == 2)
        {
          stateOfThread = "RUNNABLE";
        }
        else if(tcbTable[i].state == 3)
        {
          stateOfThread = "BLOCKED";
        }
        else if(tcbTable[i].state == 4)
        {
          stateOfThread = "EXIT";
        }
        printf("%s\t%d\t\t%s\t\t%la\n", tcbTable[i].thread_name, i, stateOfThread, tcbTable[i].run_time); 
    }
    printf("\n");
}