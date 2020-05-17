#include <stdbool.h>

typedef struct gameData { 
    char lastSymbol; 
    char board[9];
} gameData;

typedef enum clientState { None, Init, Wait, Play } clientState;

typedef struct client {
    int fd;
    clientState state;
    client * opponent;
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
        int socket;
    } value;
} event;

typedef enum msgType { Disconnect, 
Start, Move, EndOfGame, StateOfGame,
Ping, ServerFull, Wait, NickNotAvailable
} msgType;

typedef struct msg {
    msgType type;
    union msgValue {
        struct { char nick[20]; char symbol; } startInfo;
        int move;
        gameData state;
        char end;
    } value;
} msg;
