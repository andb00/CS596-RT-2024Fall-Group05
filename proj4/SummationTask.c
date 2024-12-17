#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define __NR_wait 399
void sighandler(int signum){
	printf("SUMMATION: WAITING\n");
	syscall(__NR_wait);
}
	
int main(int argc, char* argv[]){
  printf("STARTING SUMMATION WITH pid %d\n",getpid());
signal(SIGUSR1,sighandler);
  int sum = 1;
  while(1){
    sum = sum;
    int a = rand() % 2 + 1;
    sum += a;
   if (sum % 40000000 == 0){
     printf("SUM REACHED %d\n",sum);
}
}
}
