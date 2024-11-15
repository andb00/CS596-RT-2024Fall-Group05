#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define __NR_set_rsv 397
#define __NR_cancel_rsv 398
#define __NR_wait 399

int main(int argc, char* argv[]){
  int i;
  if (argc != 3){
    return -1;
  }
  int param1 = atoi(argv[1]);
  int param2 = atoi(argv[2]);

  struct timespec *C = (struct timespec*)malloc(sizeof(struct timespec));
  struct timespec *T = (struct timespec*)malloc(sizeof(struct timespec));  
  C->tv_sec = 0;
  T->tv_sec = 0;
  C->tv_nsec = param1 * 1000000 * 100;
  T->tv_nsec = param2 * 1000000 * 100;
  

  pid_t pid = getpid();


  int rv = syscall(__NR_set_rsv, pid, C, T);

  printf("SETRSV RES: %d, PID: %d\n", rv, pid);

  for(i = 0; i < 10; i++){
     rv = syscall(__NR_wait);

     printf("WAIT RES: %d, PID: %d\n",rv, pid);
  }
  rv = syscall(__NR_cancel_rsv,pid);
  printf("CANCEL RES: %d, PID: %d\n", rv, pid);
  return 0;
}
