#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define __NR_set_rsv 397
#define __NR_cancel_rsv 398
#define __NR_wait 399

int main(int argc, char* argv[]){
  int i;
  if (argc != 2){
    return -1;
  }
  int param1 = atoi(argv[1]);

  pid_t pid = param1;

  int rv = syscall(__NR_cancel_rsv,pid);
  printf("CANCEL RES: %d, PID: %d\n", rv, pid);
  return 0;
}
