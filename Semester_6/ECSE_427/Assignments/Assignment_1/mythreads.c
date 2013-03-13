#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <slack/std.h>
#include <slack/list.h>
#include "mythreads.h"

#define DEBUG 1

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
  
  //If an error occured, return -1. Otherwise, return the  thread id.
  if(!tcbTable[threadId].context.uc_stack.ss_sp){
    //We failed to allocate memory for the stack
#if DEBUG == 1
printf("Failed to allocate memory for the stack\n");
#endif
    return -1;
  }else{
#if DEBUG == 1
//printf("state of thread %s is %d\n",tcbTable[threadId].thread_name,tcbTable[threadId].state);
#endif
    int tmp = threadId;
    threadId++;
    return tmp;
  }
}

/*
This function is called at the end of the function that was 
invoked by the thread.
*/
void mythread_exit(){
  tcbTable[runningThreadId].state = EXIT; //Set the state of the thread to EXIT.
}

/*
Starts running one of the threads in the runqueue.
*/
void runthreads(){
  sigemptyset(&blockSet); //Create the empty set to hold the signals to block.
  sigaddset(&blockSet, SIGALRM); //Block the SIGALARM signal in particular.
  sigprocmask(SIG_BLOCK, &blockSet, NULL); //Blocks the signals present in the set.
  sigset(SIGALRM,&scheduler); //Assigns a handler for SIGALARM signals.
  
  //Create the timer for when threads should be switched.
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = quantum_size;
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = quantum_size;
  //setitimer(ITIMER_REAL, &timer, 0);

  //Setup first thread that will run from runqueue.
  runningThreadId = list_shift_int(runqueue); //Get the index of the thread that will run first.
#if DEBUG == 1
  printf("Thread %d will run first.\n",runningThreadId);
#endif
  tcbTable[runningThreadId].state = RUNNING; //Change it's state from RUNNABLE TO RUNNING.
  clock_gettime(CLOCK_REALTIME, &tcbTable[runningThreadId].start); //Make a note of the start time for the thread that will run.
  
  //Swap context from main to the thread that will run.
#if DEBUG == 1
  printf("main: swapcontext(&uctx_main, &tcbTable[runningThreadId].context)\n");
#endif
  if(swapcontext(&uctx_main, &tcbTable[runningThreadId].context) == -1){
    perror("swapcontext");
  }
  //list_destroy(&runqueue);
}

void set_quantum_size(int size){
  quantum_size = size;
}

void scheduler(){
  sigprocmask(SIG_BLOCK, &blockSet, NULL);
  if(list_empty(runqueue) && tcbTable[runningThreadId].state==EXIT){
    //There are no more threads in the runqueue to run so we turn off the timer
    //and go back to main.
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
#if DEBUG == 1
printf("No more tasks left. Going back to main.\n");
#endif
    swapcontext(&tcbTable[runningThreadId].context, &uctx_main);
  }else{
    //Get the id of the next thread to run.
    int nextRunningThreadId = list_shift_int(runqueue);
    int previousRunningThreadId = runningThreadId;
    runningThreadId = nextRunningThreadId; 
    
    //Update the control block of the previously active thread.
    if(tcbTable[previousRunningThreadId].state != EXIT && tcbTable[previousRunningThreadId].state != BLOCKED){
      //If the previously running thread (the one whose context we will switch from) is has not completed or
      //is not blocked, we append it to the end of the runqueue.
      runqueue = list_append_int(runqueue, previousRunningThreadId);
      //We set the status of this thread to RUNNABLE.
      tcbTable[previousRunningThreadId].state = RUNNABLE;
    }
    
    //Update the timing parameter of the
    //control block of the thread that will run next and swap contexts.
    tcbTable[runningThreadId].state = RUNNING;
    clock_gettime(CLOCK_REALTIME, &tcbTable[runningThreadId].end);
    tcbTable[runningThreadId].run_time += tcbTable[runningThreadId].run_time + tcbTable[runningThreadId].end.tv_nsec - tcbTable[runningThreadId].start.tv_nsec;
    clock_gettime(CLOCK_REALTIME, &tcbTable[runningThreadId].start);
    sigprocmask(SIG_UNBLOCK, &blockSet, NULL);
    swapcontext(&tcbTable[previousRunningThreadId].context, &tcbTable[nextRunningThreadId].context);
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
    sigprocmask(SIG_BLOCK, &blockSet, NULL);
    semTable[semaphore].value--;
    if(semTable[semaphore].value<0){
      tcbTable[runningThreadId].state = BLOCKED;
      semTable[semaphore].thread_queue = list_append_int(semTable[semaphore].thread_queue,runningThreadId);
    }
    sigprocmask(SIG_UNBLOCK, &blockSet, NULL);
}

void semaphore_signal(int semaphore){
    sigprocmask(SIG_BLOCK, &blockSet, NULL);
    semTable[semaphore].value++;
    if(semTable[semaphore].value >= 0){
      int nextThreadToRunId = list_shift_int(semTable[semaphore].thread_queue);
      tcbTable[nextThreadToRunId].state = RUNNABLE;
      runqueue = list_append_int(runqueue, nextThreadToRunId);
    }
    sigprocmask(SIG_UNBLOCK, &blockSet, NULL);
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