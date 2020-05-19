#include "definitions.h"

#define OPP_COL "\033[01;31m"
#define MY_COL "\033[01;36m"
#define DEF_COL "\033[0m"
#define HELP_COL "\033[01;32m"

int sockfd;
int epfd;
char * nick;
char * opponentNick;
char mySymbol;
gameData gameState;

void error(char * message) {
    if (errno) {
        perror(message);
        exit(EXIT_FAILURE);
    }
    return;
}

char* randomName() {
    char* name = calloc(12, sizeof(char));
    sprintf(name, ".socket");
    for (int i = 7; i < 11; i++) {
        char letter = 'a' + (rand() % 26);
        name[i] = letter;
    }
    name[11] = '\0';
    return name;
}

int initUnixSocket(char * path) {
    struct  sockaddr_un local;
    memset(&local, 0, sizeof local);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, path);
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(fd == -1) {
        error("While creating unix socket");
    }

    struct sockaddr_un bindLocal;
    memset(&bindLocal, 0, sizeof bindLocal);
    bindLocal.sun_family = AF_UNIX;
    char * sockName = randomName();
    unlink(sockName);
    strncpy(bindLocal.sun_path, sockName, sizeof bindLocal.sun_path);
    if(bind(fd, (struct sockaddr *) &bindLocal, sizeof bindLocal) == -1) {
        error("While binding unix socket");
    }

    if(connect(fd, (struct sockaddr *) &local, sizeof local) == -1) {
        error("While connecting unix socket");
    } 
    return fd;
}

int initWebSocket(char* ipv4, int port) {
    struct sockaddr_in web;
    memset(&web, 0, sizeof(web));
    web.sin_family = AF_INET;
    web.sin_port = htons(port);
    if(inet_pton(AF_INET, ipv4, &web.sin_addr) <= 0) {
        error("Invalid ip adress");
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        error("While creating web socket");
    }
    if(connect(fd, (struct sockaddr *) &web, sizeof web) == -1) {
        error("While connecting web socket");
    } 
    return fd;
}

void clearLine() {
    printf("\33[A\33[2K\r");
}

static const char symbolLine[] = 
"\t%s|%s|%s\n";

static const char crossLine[] = 
"\t─┼─┼─\n";

static const char board[] =
"\t 1 | 2 | 3 \n"
"\t─── ─── ───\n"
"\t 4 | 5 | 6 \n"
"\t─── ─── ───\n"
"\t 7 | 8 | 9 \n";

char * changeColor(char symbol) {
    char * result = calloc(20, sizeof(char));
    if(symbol == '-'){
        sprintf(result, DEF_COL" "); //none color
    }
    else if(symbol == mySymbol) {
        sprintf(result, MY_COL"%c"DEF_COL, symbol);
    }
    else {
        sprintf(result, OPP_COL"%c"DEF_COL, symbol);
    }
    return result;
}

void displayState() {
    if(gameState.whoseTurn == mySymbol) {
        printf(DEF_COL"You turn.\n");
    }
    else {
        printf(DEF_COL"Wait for your turn.\n");
    }
    for(int i = 0; i < 3; i++) {
        char * first = changeColor(gameState.board[i][0]);
        char * second = changeColor(gameState.board[i][1]);
        char * third = changeColor(gameState.board[i][2]);
        printf(symbolLine, first, second, third);
        if(i != 2) {
            printf(crossLine);
        } 
    }
}

void updateState() {
    for(int i = 0; i < 6; i++) {
        clearLine();
    }
    displayState();
}


void initDisplay() {
    printf("Your nick is %s\nYour opponent's nick: %s\n\n", nick, opponentNick);
    printf(HELP_COL"Numbers of cells: \n");
    printf(board);
    printf("\nYou sign is: "MY_COL"%c\n\n"DEF_COL, mySymbol);
    printf("\033[s"); //save cursor
    displayState();
}

void sigintHandler(int signal) {
    msg newMsg;
    newMsg.type = Disconnect;
    send(sockfd, &newMsg, sizeof newMsg, 0);
    exit(0);
}

int main(int argc, char **argv) {
    if(!strcmp(argv[2], "web") && argc == 5) {
        sockfd = initWebSocket(argv[3], atoi(argv[4]));
    }
    else if(!strcmp(argv[2], "unix") && argc == 4) {
        sockfd = initUnixSocket(argv[3]);
    }
    else {
        printf("Wrong arguments!");
        exit(0);
    }

    signal(SIGINT, sigintHandler);

    msg newMsg;
    newMsg.type = Connect;
    strcpy(newMsg.value.nick, argv[1]);
    send(sockfd, &newMsg, sizeof newMsg, 0);

    if((epfd = epoll_create1(0)) == -1){
        error("Creating epoll");
    }

    struct epoll_event stdinEvent;
    stdinEvent.events = EPOLLIN | EPOLLPRI;
    stdinEvent.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &stdinEvent);

    struct epoll_event socketEvent;
    socketEvent.events = EPOLLIN | EPOLLPRI | EPOLLHUP;
    socketEvent.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &socketEvent);

    struct epoll_event events[2];
    while(true) {
        int numberOfEvents = epoll_wait(epfd, events, 10, -1);
        if(numberOfEvents == -1) {
            error("In epoll_wait");
        }
        for(int i = 0; i < numberOfEvents; i++) {
            if(events[i].data.fd == STDIN_FILENO) {
                int move;
                clearLine();
                if(scanf("%d", &move) != 1) {
                    char x;
                    while((x = getchar()) != EOF && x != '\n'); //pomin niewłaciwe wyjście
                    continue;
                }
                if(move < 1 || move > 9) {
                    continue;
                }
                move -= 1;
                if(gameState.board[move / 3][move % 3] == '-') {
                    msg newMsg;
                    newMsg.type = Move;
                    newMsg.value.move = move;
                    sendto(sockfd, &newMsg, sizeof newMsg, 0, NULL, sizeof(struct sockaddr_in));
                }
            } 
            else {
                msg newMsg;
                recvfrom(sockfd, &newMsg, sizeof newMsg, 0, NULL, 0);
                if (newMsg.type == Wait) {
                    printf("Wait for an opponent\n");
                }
                else if (newMsg.type == StartOfGame) {
                    // fprintf(stderr, "here\n");
                    opponentNick = newMsg.value.startInfo.nick;
                    mySymbol = newMsg.value.startInfo.symbol;
                    initDisplay();
                }
                else if (newMsg.type == NickNotAvailable) {
                    printf("Nick is not available\n");
                    close(sockfd);
                    exit(0);
                }
                else if (newMsg.type == FullServer) {
                    printf("Server is full. There is no place for you!\n");
                    close(sockfd);
                    exit(0);
                }
                else if(newMsg.type == Disconnect) {
                    exit(0);
                }
                else if (newMsg.type == Ping) {
                    send(sockfd, &newMsg, sizeof newMsg, 0);
                }
                else if (events[i].events & EPOLLHUP) {
                    printf("You were disconnected\n\n");
                    exit(0);
                }
                else if (newMsg.type == StateOfGame) {
                    memcpy(&gameState, &newMsg.value.state, sizeof(gameData));
                    updateState();
                }
                else if (newMsg.type == EndOfGame) {
                    if(newMsg.value.winner == mySymbol) {
                        printf("You won!!!\n");
                    }
                    else if (newMsg.value.winner == '-') {
                        printf("Draw. No one won.\n");
                    }
                    else {
                        printf("You lost :(\n");
                    }
                    close(sockfd);
                    exit(0);
                }
            }
        }
    }
}
