#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#define _POSIX_C_SOURCE 199399L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_ORDERS 8

#define MAKERS 2
#define PACKERS 1
#define SENDERS 1

#define MAKE 1
#define PACK 2
#define SEND 3

typedef struct {
    int size;
    int status;
} order;

union semun {
    // from (Linux-specific)
    int              val;    // Value for SETVAL
    struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET
    unsigned short  *Array;  // Array for GETALL, SETALL
    struct seminfo  *__buf;  // Buffer for IPC_INFO
};

order * openShMemory(int id);
int closeShMemory(order * orders);
int deleteShMemory(int semId);

void semaphoreExecute(int semId, int index, int operation, short flag);
void semaphoreIncrease(int setId, int index);
void semaphoreDecrease(int setId, int index);
void waitForSemaphore(int setId, int index);
int getValueFromSemaphore(int setId, int index);
void deleteSemaphores(int setId);

#endif


