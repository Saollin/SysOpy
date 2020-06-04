#include "operations.h"
#include <signal.h>

key_t shMemoryKey;
int shMemoryID;

key_t semaphoresKey;
int semaphoresID;

int workers = MAKERS + PACKERS + SENDERS;
int shMemorySize = MAX_ORDERS + 1;

order * orders;

void sigintHandler(int signal) {
    kill(0, SIGINT);

    deleteSemaphores(semaphoresID);
    closeShMemory(orders);
    deleteShMemory(shMemoryID);
    exit(0);
}

order * createShMemory(size_t size) {
    shMemoryKey = ftok(getenv("HOME"), rand() % 256);
    shMemoryID = shmget(shMemoryKey, size * sizeof(order), IPC_CREAT | 0666);
    return shmat(shMemoryID, NULL, 0);
}

void createSemaphores(int semaphoresSize) {
    semaphoresKey = ftok(getenv("HOME"), rand() % 256);
    semaphoresID = semget(semaphoresKey, semaphoresSize, IPC_CREAT | 0666);
    
    union semun arg;
    arg.val = 0;

    for (int i = 0; i < semaphoresSize; ++i) {
        semctl(semaphoresID, i, SETVAL, arg);
    }

    struct sembuf * bufs = calloc(semaphoresSize, sizeof(struct sembuf));
    for (int i = 0; i < semaphoresSize; i++) {
        bufs[i].sem_num = i;
        bufs[i].sem_op = 1;
    }
    semop(semaphoresID, bufs, semaphoresSize);
}

int * createWorkers() {
    char ** arguments = calloc(5, sizeof(char *));
    for(int i = 0; i < 5; i++){
        arguments[i] = calloc(16, sizeof(char));
    }
    sprintf(arguments[1], "%d", shMemoryKey);
    sprintf(arguments[2], "%d", semaphoresKey);
    arguments[4] = NULL;

    int semID = 0;
    //makers
    int * makers = calloc(MAKERS, sizeof(int));
    for(int i = 0; i < MAKERS; i++) {
        makers[i] = semID;
        sprintf(arguments[0], "maker");
        sprintf(arguments[3], "%d", semID++);
        if(fork() == 0) {
            execv("maker", arguments);
        }
    }

    //packers
    for(int i = 0; i < PACKERS; i++) {
        sprintf(arguments[0], "packer");
        sprintf(arguments[3], "%d", semID++);
        if(fork() == 0) {
            execv("packer", arguments);
        }
    }

    //senders
    for(int i = 0; i < SENDERS; i++) {
        sprintf(arguments[0], "sender");
        sprintf(arguments[3], "%d", semID++);
        if(fork() == 0) {
            execv("sender", arguments);
        }
    }

    return makers;
}

bool isIn(int * cont, int size, int value) {
    for (int i = 0; i < size; ++i) {
        if (cont[i] == value) return true;
    }
    return false;
}

int main() {
    signal(SIGINT, sigintHandler);

    srand(time(NULL));

    orders = createShMemory(shMemorySize);
    createSemaphores(workers);

    orders[0].size = 1;
    orders[0].status = -1;

    printf("Key of shared memory: %d\n Key to set of semaphores: %d\n", shMemoryKey, semaphoresKey);

    int * makers = createWorkers();

    int workerID = 0;
    while (true) {
        if (orders[orders[0].size].status == 0) {
            workerID++;
            workerID %= workers;
            semaphoreDecrease(semaphoresID, workerID);
        }
        else {
            do {
                workerID++;
                workerID %= workers;
            } while (isIn(makers, MAKERS, workerID));
            semaphoreDecrease(semaphoresID, workerID);
        }
        sleep(1);
    }
}


