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
        int number;
        bool found = false;
        for(int i = 0; i < shMemorySize; i++) {
            int index = (i + orders[0].size) % shMemorySize;
            if(orders[index].status == MAKE) {
                found = true;
                number = 2 * orders[index].size;
                orders[index].size = number;
                orders[index].status = PACK;
                break;
            }
        }

        if(found) {
            pid_t pid = getpid();
            int makeNumber = countMAKE(orders);
            int packNumber = countPACK(orders);
            printf("(%d %ld) Przygotowałem zamówienie o wielkości: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d\n", pid, now(), number, makeNumber, packNumber);
        }
        semaphoreIncrease(semaphoresID, mySemaphore);
    }


}