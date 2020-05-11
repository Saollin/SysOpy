#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const int MAX_WAITING_TIME = 4;

int chairsNumber, clientsNumber;
int *waitRoom;
int servedClient;
bool areClients = false;

void* barber() {
    while(true) {
        pthread_mutex_lock(&mutex);
        int clients = 0;
        int placeIndex = 0;
        for(int i = 0; i < chairsNumber; i++) {
            if(waitRoom[i] != 0) {
                servedClient = waitRoom[i];
                placeIndex = i;
                clients++;
            }
        }
        if(clients == 0) {
            areClients = false;
        }
        else {
            waitRoom[placeIndex] = 0;
        }

        while(!areClients) {
            printf("Golibroda: idę spać\n");
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        printf("Golibroda: czeka %d klientów, golę klienta %d\n", clients, servedClient);
        sleep(rand() % MAX_WAITING_TIME);
    }
    return NULL;
}

void* client(void * arg) {
    int id = *((int *) arg);
    int freeChairs = 0;
    while(!freeChairs) {
        pthread_mutex_lock(&mutex);
        if(!areClients) {
            areClients = true;
            servedClient = id;
            pthread_cond_broadcast(&cond);
            printf("Budzę golibrodę; %d\n", id);
        }
        else {
            freeChairs = 0;
            bool hasSat = false;
            for(int i = 0; i < chairsNumber; i++) {
                if(waitRoom[i] == 0) {
                    freeChairs++;
                    if(!hasSat) {
                        hasSat = true;
                        waitRoom[i] =  id;
                    }
                }
            }
            if(freeChairs != 0) {
                printf("Poczekalnia, wolne miejsca: %d; %d\n", freeChairs - 1, id);
            }
            else {
                printf("Zajęte; %d\n", id);
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep((rand() % MAX_WAITING_TIME) + 1);
    }
    return NULL;
}

void * getPointer(int val) {
    int * pointer = calloc(1, sizeof(int));
    *pointer = val;
    return pointer;
}

int main(int argc, char ** argv) {
    if(argc != 3) {
        fprintf(stderr, "Wrong number of arguments!\n");
        return -1;
    }
    srand(time(NULL));
    chairsNumber = atoi(argv[1]);
    clientsNumber = atoi(argv[2]);
    
    waitRoom = calloc(chairsNumber, sizeof(int));
    pthread_t barberThread;
    pthread_t * clientsThreads = calloc(clientsNumber, sizeof(pthread_t));

    pthread_create(&barberThread, NULL, barber, NULL);
    for(int i = 0; i < clientsNumber; i++) {
       pthread_create(&clientsThreads[i], NULL, client, getPointer(i));
       sleep(rand() % MAX_WAITING_TIME); 
    }

    for(int i = 0; i < clientsNumber; i++) {
        pthread_join(clientsThreads[i], NULL);
    }
    pthread_join(barberThread, NULL);
    return 0;
}
