
#include <sys/types.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

union semun {
    // from (Linux-specific)
    int              val;    // Value for SETVAL
    struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET
    unsigned short  *Array;  // Array for GETALL, SETALL
    struct seminfo  *__buf;  // Buffer for IPC_INFO
};

int error(char * message);
int createSemaphore();
void semaphoreExecute(int semId, int operation, short flag);
void semaphoreIncrease(int semId);
void semaphoreDecrease(int semId);
void deleteSemaphores(int semId);