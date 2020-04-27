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
    }


}