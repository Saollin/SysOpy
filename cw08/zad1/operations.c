#include "operations.h"

int error(char * message) {
    if (errno) {
        perror(message);
        return 1;
    }
    return 0;
}

int createSemaphore() {
    key_t semaphoresKey = ftok(getenv("HOME"), rand() % 256);
    int semaphoreID = semget(semaphoresKey, 1, IPC_CREAT | 0666);
    if(error("While creatin semaphore")){
        exit(1);
    }
    
    union semun arg;
    arg.val = 0;
    semctl(semaphoreID, 0, SETVAL, arg);

    semaphoreIncrease(semaphoreID);
    return semaphoreID;
}

void semaphoreExecute(int semId, int operation, short flag) {
    struct sembuf * buf = calloc(1, sizeof(struct sembuf));
    buf->sem_op = operation;
    buf->sem_num = 0;
    buf->sem_flg = flag;
    semop(semId, buf, 1);
    if(error("While making operation on semaphore")){
        exit(1);   
    }
}

void semaphoreIncrease(int semId) {
    semaphoreExecute(semId, 1, 0);
}

void semaphoreDecrease(int semId) {
    semaphoreExecute(semId, -1, 0);
}

void deleteSemaphores(int semId) {
    semctl(semId, 0, IPC_RMID);
    if(error("While deleting semaphore")){
        exit(1);   
    }
}