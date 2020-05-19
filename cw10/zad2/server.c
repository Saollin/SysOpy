#include "definitions.h"

const int maxConnections = 16;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epfd;
client * waitingClient = NULL;
client ** clients;

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

void sendGameData(client * to) {
    msg newMsg;
    newMsg.type = StateOfGame;
    memcpy(&newMsg.value.state, to->game, sizeof (gameData));
    sendto(to->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &to->addr, to->addrLength);
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
    sendto(first->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &first->addr, first->addrLength);

    strcpy(newMsg.value.startInfo.nick, first->nick);
    second->symbol = newMsg.value.startInfo.symbol = secondSymbol;
    sendto(second->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &second->addr, second->addrLength);

    first->game->whoseTurn = 'x';
    sendGameData(first);
    sendGameData(second);
}

void initNewClient(union sAddr * addr, socklen_t length, int clientfd, char * nick) {
    pthread_mutex_lock(&mutex);
    int index = -1;
    for(int i = 0; i < maxConnections; i++) {
        if(clients[i]->state == None) {
            index = i;
        }
        else if(!strcmp(nick, clients[i]->nick)) {
            pthread_mutex_unlock(&mutex);
            msg newMsg;
            newMsg.type = NickNotAvailable;
            sendto(clientfd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) addr, length);
            return;
        }
    }
    if(index == -1) {
        pthread_mutex_unlock(&mutex);
        printf("Server is full\n");
        msg newMsg;
        newMsg.type = FullServer;
        sendto(clientfd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) addr, length);
        return;
    }
    printf("New player: %s\n", nick);
    client* newClient = clients[index];
    newClient->fd = clientfd;
    memcpy(&newClient->addr, addr, length);
    newClient->addrLength = length;
    newClient->isActive = true;
    strcpy(newClient->nick, nick);
    if(waitingClient != NULL) {
        pairClients(newClient, waitingClient);
        waitingClient = NULL;
    }
    else {
        printf("Player: %s is waiting\n", nick);
        waitingClient = newClient;
        newClient->state = Waiting;
        msg newMsg;
        newMsg.type = Wait;
        sendto(clientfd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) addr, length);
    }
    pthread_mutex_unlock(&mutex);
}

void deleteClient(client * deleted) {
    printf("Player %s was deleted\n", deleted->nick);
    if(deleted == waitingClient) {
        waitingClient = NULL;
    }
    msg newMsg;
    newMsg.type = Disconnect;
    sendto(deleted->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &deleted->addr, deleted->addrLength);
    free(deleted->game);
    memset(&deleted->addr, 0, sizeof deleted->addr);
    if(deleted->opponent) {
        client * op = deleted->opponent;
        deleted->opponent = NULL;
        deleteClient(op);
    }
    deleted->opponent = NULL;
    deleted->game = NULL;
    deleted->state = None;
    deleted->nick[0] = 0;
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

void handleClientMessage(client * sender, msg received) {
    if(received.type == Ping) {
        pthread_mutex_lock(&mutex);
        sender->isActive = true;
        pthread_mutex_unlock(&mutex);
    }
    else if (received.type == Move) {
        int move = received.value.move;
        gameData * game = sender->game;
        if(game->whoseTurn == sender->symbol
        && game->board[move / 3][move % 3] == '-'
        && 0 <= move && move <= 8) {
            game->board[move /3][move % 3] = sender->symbol;
            game->whoseTurn = sender->opponent->symbol;
            sendGameData(sender);
            sendGameData(sender->opponent);
            msg newMsg;
            if (checkWin(sender)) {
                fprintf(stderr, "h\n");
                newMsg.type = EndOfGame;
                newMsg.value.winner =  sender->symbol;
            }
            else if (checkDraw(sender)) {
                newMsg.type = EndOfGame;
                newMsg.value.winner = '-';
            }
            if(newMsg.type == EndOfGame) {
                sender->opponent->opponent = NULL;
                sendto(sender->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &sender->addr, sender->addrLength);
                sendto(sender->opponent->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &sender->opponent->addr, sender->opponent->addrLength);
            }
        }
        else {
            sendGameData(sender);
        }
    }
    else if(received.type == Disconnect) {
        pthread_mutex_lock(&mutex);
        deleteClient(sender);
        pthread_mutex_unlock(&mutex);
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
    struct epoll_event newEvent;
    newEvent.events = EPOLLIN | EPOLLPRI;
    newEvent.data.fd = sockfd;
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
                    sendto(clients[i]->fd, &newMsg, sizeof newMsg, 0, (struct sockaddr *) &clients[i]->addr, clients[i]->addrLength);
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

int findClientIndex(union sAddr * addr, socklen_t length) {
    for(int i = 0; i < maxConnections; i++) {
        if(!memcmp(&clients[i]->addr, addr, length)) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char ** argv) {
    if(argc != 3) {
        printf("Wrong number of arguments!\n");
        exit(0);
    }
    int portNumber = atoi(argv[1]);
    char * pathOfSocket = argv[2];
    init();

    struct sockaddr_in web;
    web.sin_family = AF_INET;
    web.sin_port = htons(portNumber);
    web.sin_addr.s_addr = htonl(INADDR_ANY);
    int webSock = socket(AF_INET, SOCK_DGRAM, 0);
    if(webSock == -1){
        error("Failed creating web socket");
    }
    initSocket(webSock, &web, sizeof web);
    printf("Creating web socket on port: %d\n", portNumber);

    struct sockaddr_un local;
    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, pathOfSocket, sizeof(local.sun_path));
    
    int localSock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(localSock == -1){
        error("Failed creating local socket");
    }
    unlink(pathOfSocket);
    initSocket(localSock, &local, sizeof local);
    printf("Creating local socket with path: %s\n", pathOfSocket);

    pthread_t pingThread;
    pthread_create(&pingThread, NULL, pingFunc, NULL);

    struct epoll_event events[10];
    while(true) {
        int numberOfEvents = epoll_wait(epfd, events, 10, -1);
        if(numberOfEvents == -1) {
            error("Failed in epoll_wait");
        }
        for(int i = 0; i < numberOfEvents; i++) {
            int sockfd = events[i].data.fd;
            msg newMsg;
            union sAddr from;
            socklen_t length = sizeof(from);
            recvfrom(sockfd, &newMsg, sizeof newMsg, 0, (struct sockaddr*) &from, &length);
            if(newMsg.type == Connect) {
                initNewClient(&from, length, sockfd, newMsg.value.nick);
            }
            else {
                int index = findClientIndex(&from, length);
                handleClientMessage(clients[index], newMsg);
            }
        }
    }
}