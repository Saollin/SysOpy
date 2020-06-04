#include "worker_common.h"
#include <signal.h>

order * orders;

int workers = MAKERS + PACKERS + SENDERS;
int shMemorySize = MAX_ORDERS + 1;

void sigintHandler(int signal) {
    closeShMemory(orders);
    exit(0);
}

int main(int argc, char ** argv) {
    signal(SIGINT, sigintHandler);

    srand(time(NULL));
    
    int shMemoryID = shmget(atoi(argv[1]), 0, 0666);
    orders = openShMemory(shMemoryID);
    int semaphoresID = semget(atoi(argv[2]), 0, 0666);
    int mySemaphore = atoi(argv[3]);

    while(true) {
        wait(semaphoresID, mySemaphore);
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

        semaphoreIncrease(semaphoresID, mySemaphore);
    }


}