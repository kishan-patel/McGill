#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <slack/std.h>
#include <slack/list.h>
#include "mythreads.h"

//Set to 1 to enable print statements to be printed for debuggin purposes.
#define DEBUG 0

//Static variables.
static ControlBlock tcbTable[THREAD_MAX];
static Semaphore semTable[SEMAPHORE_MAX];
static List * runqueue;
static ControlBlock * current_thread;
static int threadId = 0;
static int runningThreadId = 0;
static ucontext_t uctx_main;
static int quantum_size;
struct itimerval timer;
static int current_threads;
static int my_threads_init;
static int current_semaphores;
static sigset_t blockSet;  //Will hold the set of signals.
static int done = 0;
static int totalThreadsCreated = 0;
static int totalThreadsExited = 0;
long long int switch_ctr;

/*
This function initializes all the globa data structures for the thread system
*/
int mythread_init(){
  int i=0;
  
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
    while(!list_empty(semTable[0].thread_queue) || (totalThreadsExited<totalThreadsCreated));
 

    sigprocmask(SIG_BLOCK, &x, NULL);

    int i=0;
    for(i = 0; i < 10 ; i++) {
        tcbTable[i].state = EXIT;
    }

    printf("Back in main\n");
    fflush(stdout);
}

void set_quantum_size(int size){
  quantum_size = size;
}

void scheduler(){
    switch_ctr++;
   if (list_empty(runqueue) && tcbTable[runningThreadId].state == EXIT) {
        printf("returning to main");
                    fflush(stdout);

        setcontext(&uctx_main);
    }

    if (!list_empty(runqueue) ) {
        int old_running_thread = runningThreadId;
        runningThreadId = list_shift_int(runqueue);

        if (tcbTable[old_running_thread].state == RUNNABLE || tcbTable[old_running_thread].state == RUNNING) {
            runqueue = list_append_int(runqueue, old_running_thread);
        }

        if (swapcontext(&tcbTable[old_running_thread].context, &tcbTable[runningThreadId].context) == -1) {
            printf("swapcontext error");
            fflush(stdout);
            //exit(1);
        }
    } else {
        //printf("returning to main\n");
        //fflush(stdout);
        //setcontext(&uctx_main);
    }
}

int create_semaphore(int val){
  if (current_semaphores == SEMAPHORE_MAX){
    //We already have the maximum number of semphores allowed 
    //so we cannot create new ones.
    return -1;
  }else{
    semTable[current_semaphores].initial = val;
    semTable[current_semaphores].value = val;
    semTable[current_semaphores].thread_queue = list_create(NULL);
    int tmp = current_semaphores;
    current_semaphores++;
    return tmp;
  }
}

void semaphore_wait(int semaphore){
    sigset_t x;
    sigemptyset(&x);
    sigaddset(&x, SIGALRM);
    sigprocmask(SIG_BLOCK, &x, NULL);

    long long int old_switch_ctr = switch_ctr; 
    (semTable[semaphore]).value--;

    if((semTable[semaphore]).value<0) {
        (tcbTable[runningThreadId]).state = BLOCKED;
        (semTable[semaphore]).thread_queue = list_append_int((semTable[semaphore]).thread_queue,runningThreadId);
    }

    sigprocmask(SIG_UNBLOCK,&x,NULL);
    while(old_switch_ctr==switch_ctr);
}

void semaphore_signal(int semaphore){
      // Block Signals
    sigset_t x;
    sigemptyset (&x);
    sigaddset(&x, SIGALRM);
    sigprocmask(SIG_BLOCK, &x, NULL);
    
    /* Increase Semaphore value */
    semTable[semaphore].value = semTable[semaphore].value + 1;
    
    if (semTable[semaphore].value < 1) {
        int should_run;
        should_run = list_shift_int(semTable[semaphore].thread_queue);
        tcbTable[should_run].state = RUNNABLE;
        runqueue = list_append_int(runqueue, should_run);
    } 

    // Unblock Signals
    sigprocmask(SIG_UNBLOCK, &x, NULL);
    scheduler();
}

void destroy_semaphore(int semaphore){
  if(!list_empty(semTable[semaphore].thread_queue)){
    fprintf(stderr, "There are threads waiting on this semaphore. Thus, it cannot be destroyed\n");
    return;
  }
}

void mythread_state()
{
    printf("\nTHREADNAME\tTHREAD\tTHREAD STATE\tCPU TIME(ns)\n"); 
    fflush(stdout);
    int i;
    for(i = 0; (tcbTable[i].state != NOTCREATED) && (i < THREAD_MAX); ++i) {
        char * state_i;
        switch(tcbTable[i].state) {
            case 0:
                state_i = "NOTCREATED";
                break;
            case 1:
                state_i = "RUNNING";
                break;
            case 2:
                state_i = "RUNNABLE";
                break;
            case 3:
                state_i = "RUNNABLE";
                break;
            case 4:
                state_i = "EXIT";
                break;
            default:
                state_i = "UNDEFINED";
                break;
        }
        printf("\n%s\t%d\t%s\t\t%la\n", tcbTable[i].thread_name, i, state_i, tcbTable[i].run_time); 
    }
}