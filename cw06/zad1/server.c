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

char * intToString(int i) {
    char * str = callock(200, sizeof(char));
    sprintf(str, "%d", i);
    return str;
}

void sendMsg(int queue, char * msg, long type) {
    msgbuf message;
    message.type = type;
    strcpy(message.text, msg);
    msgsnd(queue, &message, sizeof(message.text), 0);
}

msgbuf * getMsg(int queue) {
    msgbuf * tmp = calloc(1, sizeof(msgbuf));
    msgrcv(queue, tmp, sizeof(tmp->text), 0, MSG_NOERROR);
    return tmp;
}

msgbuf * getMsgOfType(int queue, long type) {
    msgbuf * tmp = calloc(1, sizeof(msgbuf));
    msgrcv(queue, tmp, sizeof(tmp->text), type, MSG_NOERROR);
    return tmp;
}

void initHandler(msgbuf * message) {
    for (int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] == 0) {
            userQueues[i] = msgget(atoi(message->text), IPC_CREAT | 0777);
            sendMsg(userQueues[i], intToString(i), INIT);
            break;
        }
    }
}

void listHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    char * msg = callock(MESSAGE_LENGTH, sizeof(char));
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
    if(firstClient != secondClient && userQueues[secondClient] && !chats[secondClient]) {
        printf("Chat has started.\n");

        //message to first client with second client queue
        sendMsg(userQueues[firstClient], intToString(userQueues[secondClient]), CONNECT);

        //message to second client with first client queue
        sendMsg(userQueues[secondClient], intToString(userQueues[firstClient]), CONNECT);

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
    chats[clientID] = false;
}

void siginthandler(int signal) {
    char job[20];
    strcpy(job, "stop server");
    int usersCounter = 0;
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] != 0) {
            sendMsg(userQueues[i], job, STOP);
            usersCounter++;
        }
    }
    while(usersCounter > 0) {
        msgbuf * msg = getMsgOfType(serverQueue, STOP);
        stopHandler(msg);
        usersCounter--;
    }
    printf("Deleting server queue.\n");
    msgctl(serverQueue, IPC_RMID, NULL);
    exit(0);
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