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
#include <mqueue.h>

#include "common.h"

mqd_t userQueues[MAX_NUMBER_CLIENTS];
char * userNames[MAX_NUMBER_CLIENTS];
bool chats[MAX_NUMBER_CLIENTS];

mqd_t serverQueue;

int error(char * message) {
    if (errno) {
        perror(message);
        return 1;
    }
    return 0;
}

void initHandler(msgbuf * message) {
    for (int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] == 0) {
            char *userName = message->text;
            userQueues[i] = mq_open(userName, O_RDWR);
            sendMsg(userQueues[i], intToString(i), INIT);
            userNames[i] = calloc(strlen(userName), sizeof(char));
            strcpy(userNames[i], userName);
            printf("User with name %s has %d ID\n", userNames[i], i);
            break;
        }
    }
}

void listHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    char * msg = calloc(MESSAGE_LENGTH, sizeof(char));
    char * clientLine = calloc(100, sizeof(char));
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] != 0) {
            if(i == clientID) continue;
            if(chats[i]) {
                sprintf(clientLine, "Client %d: not available\n", i);
            }
            else {
                sprintf(clientLine, "Client %d: available\n", i);
            }
            strcat(msg, clientLine);
        }
    }
    sendMsg(userQueues[clientID], msg, LIST);
    printf("List of clients was send to %d\n", clientID);
}

void connectHandler(msgbuf * message) {
    int firstClient = atoi(strtok(message->text, " "));
    int secondClient = atoi(strtok(NULL, " "));
    if(firstClient != secondClient && userQueues[secondClient] != 0 && !chats[secondClient]) {
        printf("Chat has started.\n");

        //message to first client with second client queue
        sendMsg(userQueues[firstClient], userNames[secondClient], CONNECT);

        //message to second client with first client queue
        sendMsg(userQueues[secondClient], userNames[firstClient], CONNECT);
        //set chats
        chats[firstClient] = true; 
        chats[secondClient] = true;
    }   
}

void disconnectHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    printf("User with %d ID has disconnected.\n", clientID);
    chats[clientID] = false;
}

void stopHandler(msgbuf * message) {
    int clientID = atoi(message->text);
    printf("User with %d ID has logged out\n", clientID);
    userQueues[clientID] = 0;
    chats[clientID] = false;
}

void siginthandler(int signal) {
    (void)signal;
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        if(userQueues[i] != 0) {
            sendMsg(userQueues[i], "", STOP);
            msgbuf * msg = getMsg(serverQueue);
            stopHandler(msg);
        }
    }
    printf("\nDeleting server queue.\n");
    mq_close(serverQueue);
    mq_unlink(serverName);
    exit(0);
}

void setAttr() {
    struct mq_attr attr;
    mq_getattr(serverQueue, &attr);;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    mq_setattr(serverQueue, &attr, NULL);
}

int main() {
    
    signal(SIGINT, siginthandler);
    
    //cleaning userQueues and chats
    for(int i = 0; i < MAX_NUMBER_CLIENTS; i++) {
        userQueues[i] = 0;
        userNames[i] = NULL;
        chats[i] = false;
    }
    serverQueue = mq_open(serverName, O_CREAT | O_RDWR, 0644, NULL);
    if(error("Opening server queue")) return -1;
    setAttr();

    printf("Server queue name: %d\n", serverQueue);

    msgbuf * message;

    while(1) {
        message = getMsg(serverQueue);
        if(error("While getting message on server")) raise(SIGINT);
        if(message != NULL) {
            printf("Message: %s\n", message->text);
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
                    printf("Error\n");
                    break;
            }
        }
    }
}