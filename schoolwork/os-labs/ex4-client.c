// This program does not work on OSX, seems to be a OSX problem
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
	key_t mque = msgget(MSGKEY, 0777);
	if (mque == -1) perror("mque");
	msg.mtype = 1;
	strcpy(msg.mtext, "HELLO");
	msgsnd(mque, &msg, strlen(msg.mtext)+1, 0);
	msgrcv(mque, &msg, 256, 2, 0);
	printf("msg received: %s\n", msg.mtext);
}
