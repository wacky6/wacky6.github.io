#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	pid_t p1, p2;
	if ( (p1=fork()) == -1) { perror("Process Creation Fail."); exit(1); }
	if (p1==0) {
		puts("Child 1");
		return 0;
	}
	wait(0);
	if ( (p2=fork()) == -1) { perror("Process Creation Fail."); exit(1); }
	if (p2==0) {
		puts("Child 2");
		return 0;
	}
	wait(0);
	puts("Parent");
	return 0;
}
