#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <mqueue.h>
#include <string.h>


#define MESSAGE_LENGTH 8192L

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
#define MAX_MESSAGES 16
#define SERVER_ID 1111

#define NO_MESSAGES 42

const char * serverName = "/server";

char * intToString(int i) {
    char * str = calloc(200, sizeof(char));
    sprintf(str, "%d", i);
    return str;
}

void sendMsg(mqd_t queue, char * msg, char type) {
    int mesLength = strlen(msg);
    char message[mesLength + 2];
    message[0] = type;
    sprintf(message + 1, "%s", msg);
    mq_send(queue, message, sizeof(message), CHAT - type + 1);
}


msgbuf * getMsg(mqd_t queue) {
    msgbuf * tmp = calloc(1, sizeof(msgbuf));
    char answer[MESSAGE_LENGTH + 5];
    int succes = mq_receive(queue, answer, sizeof(answer), NULL);
    if(succes == -1) {
        return NULL;
    }
    tmp->type = answer[0];
    sprintf(tmp->text, "%s", answer + 1);
    return tmp;
}

#endif