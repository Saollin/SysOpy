#include "operations.h"
#include <errno.h>

order * openShMemory(int shMemoryId) {
    order * orders = shmat(shMemoryId, NULL, 0);
    return orders;
}

int closeShMemory(order * orders) {
    return shmdt(orders);
}

int deleteShMemory(int shMemoryId) {
    return shmctl(shMemoryId, IPC_RMID, NULL);
}

void semaphoreExecute(int setId, int index, int operation, short flag) {
    struct sembuf * buf = calloc(1, sizeof(struct sembuf));
    buf->sem_op = operation;
    buf->sem_num = index;
    buf->sem_flg = flag;
    semop(setId, buf, 1);
}

void semaphoreIncrease(int setId, int index) {
    semaphoreExecute(setId, index, 1, 0);
}

void semaphoreDecrease(int setId, int index) {
    semaphoreExecute(setId, index, -1, 0);
}

void deleteSemaphores(int setId) {
    semctl(setId, 0, IPC_RMID);
}

