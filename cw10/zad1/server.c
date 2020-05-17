#include "definitions.h"

const int maxConnections = 16;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epfd;
client * waitingClient = NULL;
client ** clients;

const int windConditions[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // columns
    {0, 4, 8}, {2, 4, 6} // diagonals
};

void error(char * message) {
    if (errno) {
        perror(message);
        exit(EXIT_FAILURE);
    }
    return;
}

client * initNewClient(int clientfd) {
    pthread_mutex_lock(&mutex);
    int index = -1;
    for(int i = 0; i < maxConnections; i++) {
        if(clients[0]->state == None) {
            index = i;
            break;
        }
    }
    if(index == -1) return NULL;
    client* newClient = clients[index];
    newClient->fd = clientfd;
    newClient->state = Init;
    newClient->isActive = true;
    pthread_mutex_unlock(&mutex);
    return newClient;
}

void deleteClient(client * deleted) {
    if(deleted == waitingClient) {
        waitingClient = NULL;
    }
    if(deleted->opponent) {
        client * opponent = deleted->opponent;
        opponent->opponent = NULL;
        opponent->game = NULL;
        deleteClient(deleted->opponent);
        free(deleted->game);
        deleted->opponent = NULL;
        deleted->game = NULL;
    }
    deleted->state = None;
    deleted->nick[0] = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, deleted->fd, NULL);
    close(deleted->fd);
}

void sendGameData(client * to) {
    msg newMsg;
    newMsg.type = StateOfGame;
    memcpy(&newMsg.value.state, to->game, sizeof (gameData));
    send(to->fd, &newMsg, sizeof newMsg, 0);
}

bool checkWin(client * clnt) {
    char * gameBoard = clnt->game->board;
    char symbol = clnt->symbol;
    for(int i = 0; i < 8; i++) {
        if(gameBoard[windConditions[i][0]] == symbol 
        && gameBoard[windConditions[i][1]] == symbol 
        && gameBoard[windConditions[i][2]] == symbol) {
            return true;
        }
    }
    return false;
}

bool checkDraw(client * clnt) {
    for(int i = 0; i < 9; i++) {
        if(clnt->game->board[i] == '-') {
            return false;
        }
    }
    return true;
}

void pairClients(client * first, client * second) {
    char firstSymbol = 'x';
    char secondSymbol = 'o';

    first->state = second->state = Playing;
    first->opponent = second;
    second->opponent = first;
    first->game = second->game = calloc(1, sizeof(gameData));
    memset(first->game->board, '-', 9);

    msg newMsg;
    newMsg.type = StartOfGame;
    strcpy(newMsg.value.startInfo.nick, second->nick);
    first->symbol = newMsg.value.startInfo.symbol = firstSymbol;
    send(first->fd, &newMsg, sizeof newMsg, 0);

    strcpy(newMsg.value.startInfo.nick, first->nick);
    second->symbol = newMsg.value.startInfo.symbol = secondSymbol;
    send(second->fd, &newMsg, sizeof newMsg, 0);

    if(rand() % 2 == 0) {
        first->game->whoseTurn = first->symbol;
    }
    else {
        first->game->whoseTurn = second->symbol;
    }
    
    sendGameData(first);
    sendGameData(second);
}

void handleClient(client * handledClient) {
    if(handledClient->state == Init) {
        int nickSize = read(handledClient->fd, handledClient->nick, sizeof handledClient->nick - 1);
        int j = handledClient - clients;
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < maxConnections; i++) {
            if(i != j && !strcmp(handledClient->nick, clients[i]->nick)) {
                msg newMsg;
                newMsg.type = NickNotAvailable;
                send(handledClient->fd, &newMsg, sizeof newMsg, 0);
                deleteClient(handledClient);
            }
            else {
                if(waitingClient) {
                    pairClients(handledClient, waitingClient);
                    waitingClient = NULL;
                }
                else {
                    msg newMsg;
                    newMsg.type = Wait;
                    send(handledClient->fd, &newMsg, sizeof newMsg, 0);
                    waitingClient = handledClient;
                    handledClient->state = Waiting;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    else {
        msg newMsg;
        read(handledClient->fd, &newMsg, sizeof newMsg);
        if(newMsg.type == Ping) {
            pthread_mutex_lock(&mutex);
            handledClient->isActive = true;
            pthread_mutex_unlock(&mutex);
        }
        else if (newMsg.type == Move) {
            int move = newMsg.value.move;
            gameData * game = handledClient->game;
            if(game->whoseTurn == handledClient->symbol
            && game->board[move] == '-'
            && 0 <= move && move <= 8) {
                game->board[move] = handledClient->symbol;
                game->whoseTurn = handledClient->opponent->symbol;
                sendGameData(handledClient);
                sendGameData(handledClient->opponent);
                if (checkWin(handledClient)) {
                    newMsg.type = EndOfGame;
                    newMsg.value.winner =  handledClient->symbol;
                }
                else if (checkDraw(handledClient)) {
                    newMsg.type = EndOfGame;
                    newMsg.value.winner = '-';
                }
                if(newMsg.type == EndOfGame) {
                    handledClient->opponent->opponent = NULL;
                    send(handledClient->fd, &newMsg, sizeof newMsg, 0);
                    send(handledClient->opponent->fd, &newMsg, sizeof newMsg, 0);
                }
            }
            else {
                sendGameData(handledClient);
            }
        }
    }
}

void initSocket(int sockfd, void * addr, int size) {
    if(bind(sockfd, (struct sockaddr *) addr, size) == -1) {
        error("While binding socket");
    }
    if(listen(sockfd, maxConnections));
    struct epoll_event newEvent;
    newEvent.events = EPOLLIN | EPOLLPRI;
    event * eventPtr = malloc(sizeof(event));
    newEvent.data.ptr = eventPtr;
    eventPtr->type = Socket;
    eventPtr->value.sockfd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &newEvent);
}

void * pingFunc() {
    static msg newMsg;
    newMsg.type = Ping;
    while(true) {
        sleep(8);
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < maxConnections; i++) {
            if(clients[i]->state != None) {
                if(clients[i]->isActive) {
                    clients[i]->isActive = false;
                    send(clients[i]->fd, &newMsg, sizeof newMsg, 0);
                }
                else {
                    deleteClient(clients[i]);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

void init() {
    if((epfd = epoll_create1(0)) == -1){
        error("Creating epoll");
    }
    clients = calloc(maxConnections, sizeof(client *));
    for(int i = 0; i < maxConnections; i++) {
        clients[i] = calloc(1, sizeof(client));
        clients[i]->state = None;
    }
}

int main(int argc, char ** argv) {
    if(argc != 3) {
        printf("Wrong number of arguments!\n");
        exit(0);
    }
    int portNumber = atoi(argv[1]);
    char * pathOfSocket = argv[2];
    unlink(pathOfSocket);
    init();

    struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, pathOfSocket);
    int localSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(localSock == -1){
        error("Creating local socket");
    }
    initSocket(localSock, &local, sizeof(struct sockaddr_un));

    struct sockaddr_in web;
    web.sin_family = AF_INET;
    web.sin_port = htons(portNumber);
    web.sin_addr.s_addr = htonl(INADDR_ANY);
    int webSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(webSock == -1){
        error("Creating web socket");
    }
    initSocket(webSock, &web, sizeof(struct sockaddr_in));

    pthread_t pingThread;
    pthread_create(&pingThread, NULL, pingFunc, NULL);

    struct epoll_event events[10];
    while(true) {
        int numberOfEvents = epoll_wait(epfd, events, 10, -1);
        if(numberOfEvents == -1) {
            error("In epoll_wait");
        }
        for(int i = 0; i < numberOfEvents; i++) {
            event * readEvent = events[i].data.ptr;
            if(readEvent->type = Socket) {
                int clientfd = accept(readEvent->value.sockfd, NULL, NULL);
                client * newClient = initNewClient(clientfd);
                if(newClient == NULL) {
                    msg newMsg;
                    newMsg.type = FullServer;
                    send(clientfd, &newMsg, sizeof newMsg, 0);
                    close(clientfd);
                    continue;
                } 
                event * clientData = calloc(1, sizeof(event));
                clientData->type = Client;
                clientData->value.client = newClient;
                struct epoll_event newEvent;
                newEvent.events = EPOLLIN | EPOLLET | EPOLLHUP;
                newEvent.data.ptr = clientData;
                if((epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &newEvent)) == -1) {
                    error("While epoll_ctl for new client");
                }
            }
            else if(readEvent->type = Client) {
                if(events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    deleteClient(readEvent->value.client);
                    pthread_mutex_unlock(&mutex);
                }
                else handleClient(readEvent->value.client);
            }
        }
    }
}