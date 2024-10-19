#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#define __NR_calc 397

int main(int argc, char* argv[]) {
	if (argc != 4)
		return -1;
	// runtime errors are ok for user applications
	int param1 = atoi(argv[1]);
	char operation =  argv[2][0];
	int param2 = atoi(argv[3]);
	int* result = (int*) malloc(sizeof(int));
	int rv = syscall(__NR_calc, param1, param2, operation, result);
	if (rv != 0)
		printf("NaN");
	else
		printf("%d", *result);
	return 0;
}

