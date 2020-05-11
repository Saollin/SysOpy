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

#define GOL_COL "\033[01;31m"
#define DEF_COL "\033[0m"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const int MAX_WAITING_TIME = 4;

int chairsNumber, clientsNumber, freeChairs;
int *waitRoom;
int servedClient;
bool doesBarberSleep = true;

void* barber() {
    for(int j = 0; j < clientsNumber; j++) {
        pthread_mutex_lock(&mutex);
        while(freeChairs == chairsNumber) {
            printf(GOL_COL"%9s: ", "Golibroda");
            printf(DEF_COL"Idę spać\n");
            doesBarberSleep = true;
            pthread_cond_wait(&cond, &mutex);
        }
        int placeIndex = 0;
        for(int i = 0; i < chairsNumber; i++) {
            if(waitRoom[i] != 0) {
                servedClient = waitRoom[i];
                placeIndex = i;
                break;
            }
        }
        waitRoom[placeIndex] = 0;
        freeChairs++;
        int clients = chairsNumber - freeChairs;
        char * clientForm = calloc(10, sizeof(char));
        if(clients == 1) sprintf(clientForm, "klient");
        else sprintf(clientForm, "klientów");
        printf(GOL_COL"%9s: ", "Golibroda");
        printf(DEF_COL"Czeka %d %s, golę klienta %d\n", clients, clientForm, servedClient);
        pthread_mutex_unlock(&mutex);
        sleep(rand() % MAX_WAITING_TIME);
    }
    printf(GOL_COL"%9s: ", "Golibroda");
    printf(DEF_COL"Koniec pracy\n");
    return NULL;
}

void* client(void * arg) {
    int id = *((int *) arg);
    bool wasServed = false;
    while(!wasServed) {
        pthread_mutex_lock(&mutex);
        if(freeChairs != 0) {
            for(int i = 0; i < chairsNumber; i++) {
                if(waitRoom[i] == 0) {
                    waitRoom[i] =  id;
                    break;
                }
            }
            freeChairs--;
            printf("%6s %-2d: ", "Klient", id);
            printf("Poczekalnia, wolne miejsca: %d\n", freeChairs);
            if(doesBarberSleep) {
                doesBarberSleep = false;
                pthread_cond_broadcast(&cond);
                printf("%6s %-2d: ", "Klient", id);
                printf("Budzę golibrodę\n");
            }
            wasServed = true;
        }
        else {
            printf("%6s %-2d: ", "Klient", id);
            printf("Zajęte\n");
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
    freeChairs = chairsNumber = atoi(argv[1]);
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
