#include "worker_common.h"

long int now() {
    struct timespec ts;
    clock_gettime (CLOCK_MONOTONIC, &ts);
    return  (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

int countMAKE(order * orders) {
    int counter = 0;
    for (int i = 1; i < MAX_ORDERS + 1; ++i) 
        if (orders[i].status == MAKE) {
            counter++;
        }
    return counter;
}

int countPACK(order * orders) {
    int counter = 0;
    for (int i = 1; i < MAX_ORDERS + 1; ++i) 
        if (orders[i].status == PACK) {
            counter++;
        }
    return counter;
}

void wait(int semaphoresID, int id) {
    struct sembuf *tmp = calloc(1, sizeof(struct sembuf));

    tmp->sem_num = id;
    tmp->sem_op = 0;
    
    semop(semaphoresID, tmp, 1);
}