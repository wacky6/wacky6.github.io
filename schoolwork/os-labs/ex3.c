#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void child(int child_id);
void wait_sig();
void sig_int();
int sig_int_called = 0;

int main(){
	pid_t p1, p2;
	int fd_stdout = STDOUT_FILENO;

	signal(SIGINT, sig_int);
	
	while ((p1=fork()) == -1);
	if (!p1){
		child(1);
		return 0;
	}
	while ((p2=fork()) == -1);
	if (!p2) {
		child(2);
		return 0;
	}

	puts("waiting SIGINT...");
	wait_sig();

	kill(p1, SIGINT);
	kill(p2, SIGINT);

	wait(0);
	wait(0);

	puts("parent is terminating");
	return 0;
}

void child(int child_id){
	signal(SIGINT, sig_int);
	wait_sig();
	lockf(STDOUT_FILENO, F_LOCK, 0);
	printf("child %d is terminating\n", child_id);
	lockf(STDOUT_FILENO, F_ULOCK, 0);
}

void wait_sig(){
	while(!sig_int_called) sleep(1);
}

void sig_int() {
	sig_int_called = 1;
}
