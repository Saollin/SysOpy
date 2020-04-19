#ifndef COMMON_H
#define COMMON_H

#define MESSAGE_LENGTH 256

typedef struct {
    long type;
    char text[MESSAGE_LENGTH];
} msgbuf;

#define STOP 1L
#define DISCONNECT 2L
#define LIST 3L
#define INIT 4L
#define CONNECT 5L

#define MAX_NUMBER_CLIENTS 64
#define SERVER_ID 1111

#define NO_MESSAGES 42

#endif