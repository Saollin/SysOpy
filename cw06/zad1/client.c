#define _POSIX_C_SOURCE 199309L
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
#include <time.h>

#include "common.h"

int serverQueue;
int chat = -1;
int myQueue;
int idOnServer = -1;

char * intToString(int i) {
    char * str = calloc(200, sizeof(char));
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

void stopClient() {
    sendMsg(serverQueue, intToString(idOnServer), STOP);
    if(chat != -1) {
        sendMsg(chat, intToString(idOnServer), DISCONNECT);
    }
    printf("\nQueue will be deleted.\n");
    msgctl(myQueue, IPC_RMID, NULL);
    exit(0);
}

void sigintHandler(int signal) {
    stopClient();
}

void getAnswers(union sigval sv) {
    (void)sv;
    msgbuf answer;
    while (msgrcv(myQueue, &answer, MESSAGE_LENGTH, 0, IPC_NOWAIT) != -1) {
        switch (answer.type) {
            case STOP:
                printf("Serves was closed");
                stopClient();
                break;
            case CONNECT:
                chat = atoi(answer.text);
                printf("Chat with %d has started.\n", chat);
                break;
            case DISCONNECT:
                sendMsg(serverQueue, intToString(idOnServer), DISCONNECT);
                printf("Chat with %d has finished\n", chat);
                chat = -1;
                break;
            case CHAT:
                printf("-> %s", answer.text);
                break;
            case LIST:
                printf("List of clients: \n");
                printf("%s\n", answer.text);
                break;
        }
    }
}

void messageListener() {
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = getAnswers;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ten_ms = {0, 10000000};
    struct itimerspec timer_value = {ten_ms, ten_ms};
    timer_settime(timer, 0, &timer_value, NULL);
}

int main(int argc, char ** argv) {
    char *home = getpwuid(getuid())->pw_dir;
    
    key_t serverKey = ftok(home, SERVER_ID);
    serverQueue = msgget(serverKey, 0777);

    key_t clientKey = ftok(home, getpid());
    myQueue = msgget(clientKey, IPC_CREAT | 0777);

    signal(SIGINT, sigintHandler);

    sendMsg(serverQueue, intToString(myQueue), INIT);

    idOnServer = atoi(getMsgOfType(myQueue, INIT)->text);
    // printf("here\n");
    
    printf("My queue ID: %d\n", myQueue);
    printf("My id on server: %d\n", idOnServer);

    messageListener();

    // struct msqid_ds info;
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
            sendMsg(chat, intToString(idOnServer), DISCONNECT);
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