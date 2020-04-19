#include <pwd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "common.h"

int userQueues[MAX_NUMBER_CLIENTS];
int chats[MAX_NUMBER_CLIENTS];

int serverQueue;

void sendMsg(int queue, char * msg, long type) {
    msgbuf message;
    message.type = type;
    strcpy(message.text, msg);
    msgsnd(queue, &message, sizeof(message.text), 0);
}

msgbuf * getMsg(int queue) {
    msgbuf * tmp = calloc(1, sizeof(msgbuf));
    tmp->type = 1;
    msgrcv(queue, tmp, sizeof(tmp->text), 0, MSG_NOERROR);
    return tmp;
}

void siginthandler(int signal) {
    char job[5];
    strcpy(job, "job");
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] != 0) {
            sendMsg(userQueues[i], job, DISCONNECT);
        }
    }
    msgctl(serverQueue, IPC_RMID, NULL);
    exit(0);
}


int main(int argc, char ** argv) {
    
    //cleaning userQueues and chats
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        userQueues[i] = 0;
        chats[i] = 0;
    }

    char * home = getpwuid(getuid())->pw_dir;
    
    key_t serverKey = ftok(home, SERVER_ID);
    serverQueue = msgget(serverKey, IPC_CREAT | 0777);
    printf("Server ID: %d\n", serverKey);

    signal(SIGINT, siginthandler);

    msgbuf * message;

    while(1) {
        message = getMsg(serverQueue);
        if(errno != NO_MESSAGES) {
            printf("Message: %s", message->text);
            switch(message->type) {
                case INIT:
                    initHandler();
                    break;
                case LIST:
                    listHandler();
                    break;
                case CONNECT:
                    connectHandler();
                    break;
                case DISCONNECT:
                    disconnectHandler();
                    break;
                case STOP:
                    stopHandler();
                    break;
                default:
                    printf("Error");
                    break;
            }
        }
    }
}