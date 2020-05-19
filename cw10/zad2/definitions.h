#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdbool.h>

typedef struct gameData { 
    char whoseTurn; 
    char board[3][3];
} gameData;

typedef enum clientState { None, Init, Waiting, Playing } clientState;

typedef struct client {
    int fd;
    clientState state;
    struct client * opponent;
    char nick[20];
    char symbol;
    gameData * game;
    bool isActive;
} client;

typedef enum eventState { Socket, Client } eventState;

typedef struct event {
    eventState type;
    union value {
        client * client;
        int sockfd;
    } value;
} event;

typedef enum msgType { Disconnect, 
StartOfGame, Move, EndOfGame, StateOfGame,
Ping, FullServer, Wait, NickNotAvailable
} msgType;

typedef struct msg {
    msgType type;
    union msgValue {
        struct { char nick[20]; char symbol; } startInfo;
        int move;
        gameData state;
        char winner;
    } value;
} msg;
