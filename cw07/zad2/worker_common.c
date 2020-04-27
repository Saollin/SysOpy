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