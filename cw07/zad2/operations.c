#include "operations.h"
#include <errno.h>

order * openShMemory(char * name, int size) {
    int id = shm_open(name, O_RDWR, 0);
    order * orders = mmap(NULL, size * sizeof(order), PROT_READ | PROT_EXEC | PROT_WRITE, MAP_SHARED, id, 0);
    return orders;
}

int closeShMemory(order * orders, int size) {
    return munmap(orders, size);
}

int deleteShMemory(char * name) {
    return shm_unlink(name);
}

int getValueFromSemaphore(sem_t * semId) {
    int value;
    sem_getvalue(semId, &value);
    return value;
}

void closeSemaphore(sem_t * semId) {
    sem_close(semId);
}
