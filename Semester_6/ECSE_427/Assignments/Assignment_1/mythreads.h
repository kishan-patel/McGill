#ifndef __MY_THREADS_H__
#define __MY_THREADS_H__

#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <ucontext.h>
#include <slack/std.h>
#include <slack/list.h>

#define THREAD_NAME_LEN 32
#define THREAD_MAX 128
#define SEMAPHORE_MAX 128
#define THREAD_STACK_SIZE 4096
#define QUANTUM_N_SIZE 60000

enum ThreadState {
  NOTCREATED,
  RUNNING,
  RUNNABLE,
  BLOCKED,
  EXIT
};

typedef struct ControlBlock_t {
  ucontext_t context;
  char thread_name[THREAD_NAME_LEN];
  int thread_id;
  enum ThreadState state;
  struct timespec start;
  struct timespec end;
  double run_time;
} ControlBlock;

typedef struct Semaphore_t {
  List * thread_queue;
  int value;
  int initial;
  int active;
} Semaphore;

int mythreads_init();
int mythread_create(char *threadname, void (*threadfunc)(), int stacksize);
void mythread_exit();
int mythread_id();
void runthreads();
void set_quantum_size(int quantum);
int create_semaphore(int value);
void semaphore_wait(int semaphore);
void semaphore_signal(int semaphore);
void destroy_semaphore(int semaphore);
void mythread_state();
void evict_thread();
void scheduler();

#endif /* __MY_THREADS_H__ */
