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
#include <pwd.h>
#include <mqueue.h>

#include "common.h"

mqd_t serverQueue;
mqd_t chat = -1;
mqd_t myQueue;
char * myName;
int idOnServer = -1;

int error(char * message) {
    if (errno) {
        perror(message);
        return 1;
    }
    return 0;
}

void stopClient() {
    sendMsg(serverQueue, intToString(idOnServer), STOP);
    if(chat != -1) {
        sendMsg(chat, intToString(idOnServer), DISCONNECT);
    }
    printf("\nQueue will be deleted.\n");
    mq_close(myQueue);
    mq_unlink(myName);
    exit(0);
}

void sigintHandler(int signal) {
    (void)signal;
    stopClient();
}

void getAnswers(union sigval sv) {
    (void)sv;
    msgbuf * answer;
    while ((answer = getMsg(myQueue)) != NULL) {
        switch (answer->type) {
            case STOP:
                printf("Serves was closed");
                stopClient();
                break;
            case DISCONNECT:
                sendMsg(serverQueue, intToString(idOnServer), DISCONNECT);
                printf("Chat with %d has finished\n", chat);
                mq_close(chat);
                mq_unlink(answer->text);
                chat = -1;
                break;
            case LIST:
                printf("List of clients: \n");
                printf("%s\n", answer->text);
                break;
            case CONNECT:
                chat = mq_open(answer->text, O_RDWR);
                printf("Chat with %s has started.\n", answer->text);
                break;
            case CHAT:
                printf("-> %s", answer->text);
                break;
        }
    }
}

void messageListener() {
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = getAnswers;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    
    mq_notify(myQueue, &event);
}

void setAttr() {
    struct mq_attr attr;
    mq_getattr(serverQueue, &attr);;
    attr.mq_flags = O_NONBLOCK;
    attr.mq_curmsgs = 0;
    mq_setattr(serverQueue, &attr, NULL);
}

int main() {
    myName = calloc(20, sizeof(char));
    sprintf(myName, "/client%d", getpid());
    printf("%s\n", myName);
    serverQueue = mq_open(serverName, O_RDWR);
    if(error("Opening server queue")) return -1;
    myQueue = mq_open(myName, O_RDWR | O_CREAT, 0666, NULL);
    if(error("Opening queue")) return -1;
    setAttr();

    
    printf("myqueue: %d\n", myQueue);
    signal(SIGINT, sigintHandler);

    sendMsg(serverQueue, myName, INIT);
    msgbuf * answer = getMsg(myQueue);
    if(error("Getting answer")) return -1;
    
    idOnServer = atoi(answer->text);
    printf("My queue ID: %d\n", myQueue);
    printf("My id on server: %d\n", idOnServer);
    sleep(1);

    messageListener();

    char inputLine[100];
    while(true) {

        fgets(inputLine, sizeof(inputLine), stdin);
        if(!strncmp(inputLine, "LIST", strlen("LIST"))) { //starts with
            sendMsg(serverQueue, intToString(idOnServer), LIST);
        }
        else if (!strncmp(inputLine, "CONNECT", strlen("CONNECT"))) {
            (void)strtok(inputLine, " ");
            int secondClient = atoi(strtok(NULL, " "));
            char msg[MESSAGE_LENGTH];
            sprintf(msg, "%d %d", idOnServer, secondClient);
            sendMsg(serverQueue, msg, CONNECT);
        }
        else if (!strncmp(inputLine, "DISCONNECT", strlen("DISCONNECT"))) {
            sendMsg(serverQueue, intToString(idOnServer), DISCONNECT);
            printf("Chat with %d has finished\n", chat);
            sendMsg(chat, myName, DISCONNECT);
            chat = -1;
        }
        else if (!strncmp(inputLine, "STOP", strlen("STOP"))) {
            stopClient();
        }
        else if (chat != -1) {
            sendMsg(chat, inputLine, CHAT);
        }
        sleep(1);

        
    }
    stopClient();
}