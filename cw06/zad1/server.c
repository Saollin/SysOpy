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
#include <stdbool.h> 

#include "common.h"

int userQueues[MAX_NUMBER_CLIENTS];
bool chats[MAX_NUMBER_CLIENTS];

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

void initHandler(msgbuf * message) {
    for (int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] == 0) {
            userQueues[i] = msgget(atoi(message->text), IPC_CREAT | 0777);
            char buf[50];
            sprintf(buf, "%d", i);
            sendMsg(userQueues[i], buf, INIT);
            break;
        }
    }
}

void listHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    char * msg = callock(1000, sizeof(char));
    char * clientLine = callock(100, sizeof(char));
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] != 0) {
            if(i != clientID && chats[i]) {
                sprintf(clientLine, "Client %d: not available\n", i);
            }
            else {
                sprintf(clientLine, "Client %d: available\n", i);
            }
            strcat(msg, clientLine);
        }
    }
    sendMsg(userQueues[clientID], msg, LIST);
}

void connectHandler(msgbuf * message) {
    int firstClient = atoi(strtok(message->text, " "));
    int secondClient = atoi(strtok(NULL, " "));
    char * msg = callock(200, sizeof(char));
    if(firstClient != secondClient && userQueues[secondClient] && !chats[secondClient]) {
        printf("Chat has started.\n");

        //message to first client with second client queue
        sprintf(msg, "%d", userQueues[secondClient]);
        sendMsg(userQueues[firstClient], msg, CONNECT);

        //message to second client with first client queue
        sprintf(msg, "%d", userQueues[firstClient]);
        sendMsg(userQueues[secondClient], msg, CONNECT);

        //set chats
        chats[firstClient] = chats[secondClient] = true;
    }   
}

void disconnectHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    printf("User with %d ID has disconnected.", clientID);
    chats[clientID] = false;
}

void stopHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    printf("User with %d ID has logged out\n", clientID);
    userQueues[clientID] = 0;
    chats[clientID] = 0;
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

void connectHandler(msgbuf * message) {
    
}

int main(int argc, char ** argv) {
    
    //cleaning userQueues and chats
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        userQueues[i] = 0;
        chats[i] = false;
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
                    initHandler(message);
                    break;
                case LIST:
                    listHandler(message);
                    break;
                case CONNECT:
                    connectHandler(message);
                    break;
                case DISCONNECT:
                    disconnectHandler(message);
                    break;
                case STOP:
                    stopHandler(message);
                    break;
                default:
                    printf("Error");
                    break;
            }
        }
    }
}