#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define MESSAGE_LENGTH 1000

typedef struct {
    long type;
    char text[MESSAGE_LENGTH];
} msgbuf;

#define STOP 1L
#define DISCONNECT 2L
#define LIST 3L
#define INIT 4L
#define CONNECT 5L
#define CHAT 6L

#define MAX_NUMBER_CLIENTS 64
#define SERVER_ID 1111

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

#define NO_MESSAGES 42

#endif