#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/* two threads incrementing a shared counter */

volatile int global = 0;
volatile int count = 0;

int try(volatile int *mutex) {
  return __sync_val_compare_and_swap(mutex, 0, 1);
}

int lock(volatile int *mutex) {
  int spin = 0;
  while(try(mutex) != 0) {
    spin++;
  }
  return spin;
}

void unlock(volatile int *mutex) {
  *mutex = 0;
}

typedef struct args {int inc; int id; volatile int *mutex;} args;

void *increment(void *arg) {
  int inc = ((args*)arg)->inc;
  int id = ((args*)arg)->id;
  volatile int *mutex = ((args*)arg)->mutex;

  for(int i = 0; i < inc; i++) {
    int spin = lock(mutex);
    printf("%d\n", spin);
    sched_yield();
    count++;
    unlock(mutex);
  }
}

/*left to do: fix the main so the program can be compiled without any errors*/

int main(int argc, char *argv[]) {

  if(argc != 2) {
    printf("usage peterson <inc>\n");
    exit(0);
  }

  int inc = atoi(argv[1]);  //atoi turns char(s) to int.

  pthread_t one_p, two_p;
  args one_args, two_args;

  one_args.inc = inc;
  two_args.inc = inc;
  one_args.mutex = &global;
  two_args.mutex = &global;

  pthread_create(&one_p, NULL, increment, &one_args);
  pthread_create(&two_p, NULL, increment, &two_args);
  pthread_join(one_p, NULL);
  printf("start 1\n");
  pthread_join(two_p, NULL);
  printf("start 2\n");

  printf("result is %d\n", count);
  return 0;
}
