#include "definitions.h"

const int maxConnections = 16;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epfd;
client * waitingClient = NULL;
client ** clients;
int webSock;
int localSock;
char * pathOfSocket;

int winConditions[8][3] = {
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
        if(clients[i]->state == None) {
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
    if(deleted == NULL) {
        return;
    }
    printf("Player %s was deleted\n", deleted->nick);
    if(deleted == waitingClient) {
        waitingClient = NULL;
    }
    if(deleted->opponent) {
        client * op = deleted->opponent;
        deleted->opponent->opponent = NULL;
        // disconnect opponent
        msg newMsg;
        newMsg.type = Disconnect;
        send(op->fd, &newMsg, sizeof newMsg, 0);
        
        deleteClient(op);
        deleted->opponent = NULL;
    }
    free(deleted->game);
    deleted->game = NULL;
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
    // char ** gameBoard = clnt->game->board;
    char symbol = clnt->symbol;
    for(int i = 0; i < 8; i++) {
        int * cond = winConditions[i];
        if(clnt->game->board[cond[0] / 3][cond[0] % 3] == symbol 
        && clnt->game->board[cond[1] / 3][cond[1] % 3] == symbol 
        && clnt->game->board[cond[2] / 3][cond[2] % 3] == symbol) {
            return true;
        }
    }
    return false;
}

bool checkDraw(client * clnt) {
    for(int i = 0; i < 9; i++) {
        if(clnt->game->board[i / 3][i % 3] == '-') {
            return false;
        }
    }
    return true;
}

void pairClients(client * first, client * second) {
    printf("Now player %s plays with %s\n", first->nick, second->nick);
    char firstSymbol = 'x';
    char secondSymbol = 'o';
    if(rand() % 2 == 0) {
        firstSymbol = 'o';
        secondSymbol = 'x';
    }

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

    first->game->whoseTurn = 'x';
    sendGameData(first);
    sendGameData(second);
}

void handleClient(client * handledClient) {
    if(handledClient->state == Init) {
        int nickSize = read(handledClient->fd, handledClient->nick, sizeof handledClient->nick - 1);
        pthread_mutex_lock(&mutex);
        bool isNickAvailable = true;
        for(int i = 0; i < maxConnections; i++) {
            if(handledClient != clients[i] && strcmp(handledClient->nick, clients[i]->nick) == 0) {
                isNickAvailable = false;
                break;
            }
        }
        if(!isNickAvailable){
            msg newMsg;
            newMsg.type = NickNotAvailable;
            send(handledClient->fd, &newMsg, sizeof newMsg, 0);
            deleteClient(handledClient);
        }
        else {
            handledClient->nick[nickSize] = '\0';
            printf("New player: %s\n", handledClient->nick);
            if(waitingClient != NULL) {
                pairClients(handledClient, waitingClient);
                waitingClient = NULL;
            }
            else {
                printf("Player: %s is waiting\n", handledClient->nick);
                msg newMsg;
                newMsg.type = Wait;
                send(handledClient->fd, &newMsg, sizeof newMsg, 0);
                waitingClient = handledClient;
                handledClient->state = Waiting;
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
            && game->board[move / 3][move % 3] == '-'
            && 0 <= move && move <= 8) {
                game->board[move /3][move % 3] = handledClient->symbol;
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
        else if(newMsg.type == Disconnect) {
            msg newMsg;
            newMsg.type = Disconnect;
            send(handledClient->fd, &newMsg, sizeof newMsg, 0);
            deleteClient(handledClient);
        }
    }
}

void initSocket(int sockfd, void * addr, int size) {
    int yes = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&yes, sizeof(yes)) == -1) {
        error("Failed setsockopt");
    }
    if(bind(sockfd, (struct sockaddr *) addr, size) == -1) {
        error("Failed binding socket");
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
        printf("Ping to clients\n");
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
        error("Failed to creating epoll");
    }
    clients = calloc(maxConnections, sizeof(client *));
    for(int i = 0; i < maxConnections; i++) {
        clients[i] = (client *) calloc(1, sizeof(client));
        clients[i]->state = None;
    }
}

void sigintHandler(int signal) {
    printf("\nServer closed\n");
    epoll_ctl(epfd, EPOLL_CTL_DEL, webSock, NULL);
    epoll_ctl(epfd, EPOLL_CTL_DEL, localSock, NULL);
    close(localSock);
    close(webSock);
    unlink(pathOfSocket);
    exit(0);
}

int main(int argc, char ** argv) {
    if(argc != 3) {
        printf("Wrong number of arguments!\n");
        exit(0);
    }
    int portNumber = atoi(argv[1]);
    pathOfSocket = argv[2];
    init();

    struct sockaddr_in web;
    web.sin_family = AF_INET;
    web.sin_port = htons(portNumber);
    web.sin_addr.s_addr = htonl(INADDR_ANY);
    
    webSock = socket(AF_INET, SOCK_STREAM, 0);
    if(webSock == -1){
        error("Failed creating web socket");
    }
    initSocket(webSock, &web, sizeof web);
    printf("Creating web socket on port: %d\n", portNumber);

    struct sockaddr_un local;
    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, pathOfSocket, sizeof(local.sun_path) -1);
    
    localSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(localSock == -1){
        error("Failed creating local socket");
    }
    unlink(pathOfSocket);
    initSocket(localSock, &local, sizeof local);
    printf("Creating local socket with path: %s\n", pathOfSocket);

    pthread_t pingThread;
    pthread_create(&pingThread, NULL, pingFunc, NULL);
    signal(SIGINT, sigintHandler);

    struct epoll_event events[10];
    while(true) {
        int numberOfEvents = epoll_wait(epfd, events, 10, -1);
        if(numberOfEvents == -1) {
            error("Failed in epoll_wait");
        }
        for(int i = 0; i < numberOfEvents; i++) {
            event * readEvent = events[i].data.ptr;
            if(readEvent->type == Socket) {
                int clientfd = accept(readEvent->value.sockfd, NULL, NULL);
                client * newClient = initNewClient(clientfd);
                if(newClient == NULL) {
                    printf("Server is full\n");
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
                    error("Failed epoll_ctl for new client");
                }
            }
            else if(readEvent->type == Client) {
                if(events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    deleteClient(readEvent->value.client);
                    pthread_mutex_unlock(&mutex);
                }
                else {
                    handleClient(readEvent->value.client);
                }
            }
        }
    }
}