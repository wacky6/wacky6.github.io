// This program will not work correctly on OSX, seems to be a OSX problem
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSGKEY (120)

struct msg_payload {
	long mtype;
	char mtext[256];
};

int main(){
	struct msg_payload msg;
	int pid, *pint;
	key_t mque = msgget(MSGKEY, 0777|IPC_CREAT);
	if (mque == -1) perror("mque");
	while (1) {
		msgrcv(mque, &msg, 256, 1, 0);
		printf("receive msg: %s\n", msg.mtext);
		msg.mtype=2;
		strcpy(msg.mtext, "ACK");
		msgsnd(mque, &msg, strlen(msg.mtext)+1, 0);
	}
}
