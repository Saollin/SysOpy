#include "worker_common.h"
#include <signal.h>

char * shMemoryName;
order * orders;

char * mySemaphoreName;
sem_t * mySemaphoreID;

int workers = MAKERS + PACKERS + SENDERS;
int shMemorySize = MAX_ORDERS + 1;

void sigintHandler(int signal) {
    closeShMemory(orders, shMemorySize);
    exit(0);
}

int main(int argc, char ** argv) {
    signal(SIGINT, sigintHandler);

    srand(time(NULL));
    
    shMemoryName = argv[1];
    orders = openShMemory(shMemoryName, shMemorySize);

    mySemaphoreName = argv[2];
    mySemaphoreID = sem_open(mySemaphoreName, O_RDWR);

    while(true) {
        sem_wait(mySemaphoreID);
        pid_t pid = getpid();
        int number = rand() % 10;
        int makeNumber = countMAKE(orders);
        int packNumber = countPACK(orders);
        printf("(%d %ld) Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d\n", pid, now(), number, makeNumber, packNumber);

        orders[orders[0].size].status = MAKE;
        orders[orders[0].size].size = number;
        orders[0].size++;
        orders[0].size %= shMemorySize;
        if(orders[0].size == 0) {
            orders[0].size++;
        } 
    }


}