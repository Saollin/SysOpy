#include "operations.h"
#include <signal.h>
#include <errno.h>

#define debug true

char * shMemoryName;
int shMemoryID;
int shMemorySize = MAX_ORDERS + 1;
order * orders;

char ** semaphoresNames;
sem_t ** semaphoresIDs;

int workers = MAKERS + PACKERS + SENDERS;

int error(char * message) {
    if (errno) {
        perror(message);
        return 1;
    }
    return 0;
}

void deleteSemaphores() {
    for(int i = 0; i < workers; i++) {
        sem_close(semaphoresIDs[i]);
        sem_unlink(semaphoresNames[i]);
    }
}

void sigintHandler(int signal) {
    kill(0, SIGINT);

    deleteSemaphores();
    closeShMemory(orders, shMemorySize);
    deleteShMemory(shMemoryName);
    exit(0);
}

void createShMemory(size_t size) {
    shMemoryName = calloc(16, sizeof(char));
    sprintf(shMemoryName, "/shMemory");
    shMemoryID = shm_open(shMemoryName, O_CREAT | O_RDWR, S_IRWXU);
    if(error("Creating share memory")) {
        exit(0);
    }
    printf("id %d\n", shMemoryID);
    printf("trunc: %d\n", ftruncate(shMemoryID, size * sizeof(order)));
    orders = mmap(NULL, size * sizeof(order), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, shMemoryID, 0);
}

void createSemaphores(int semaphoresNumber) {
    semaphoresNames = calloc(semaphoresNumber, sizeof(char *));
    semaphoresIDs = calloc(semaphoresNumber, sizeof(sem_t *));
    for(int i = 0; i < semaphoresNumber; i++) {
        semaphoresNames[i] = calloc(16, sizeof(char));
        sprintf(semaphoresNames[i], "/semaphore%d", i);
        semaphoresIDs[i] = sem_open(semaphoresNames[i], O_CREAT | O_CREAT, S_IRUSR|S_IWUSR, 0);
    }
}


int * createWorkers() {
    char ** arguments = calloc(5, sizeof(char *));
    for(int i = 0; i < 4; i++){
        arguments[i] = calloc(16, sizeof(char));
    }
    sprintf(arguments[1], "%s", shMemoryName);
    arguments[3] = NULL;

    int semID = 0;
    //makers
    int * makers = calloc(MAKERS, sizeof(int));
    for(int i = 0; i < MAKERS; i++) {
        makers[i] = semID;
        sprintf(arguments[0], "maker");
        sprintf(arguments[2], "%s", semaphoresNames[semID++]);
        if(fork() == 0) {
            execv("maker", arguments);
        }
    }

    //packers
    for(int i = 0; i < PACKERS; i++) {
        sprintf(arguments[0], "packer");
        sprintf(arguments[2], "%s", semaphoresNames[semID++]);
        if(fork() == 0) {
            execv("packer", arguments);
        }
    }

    //senders
    for(int i = 0; i < SENDERS; i++) {
        sprintf(arguments[0], "sender");
        sprintf(arguments[2], "%s", semaphoresNames[semID++]);
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

    createShMemory(shMemorySize);
    createSemaphores(workers);

    orders[0].size = 1;
    orders[0].status = -1;

    printf("Key of shared memory: %s\n", shMemoryName); 

    int * makers = createWorkers();

    int workerID = 0;
    while (true) {
        if (debug) {
            printf("{ \n");
            for (int i = 0; i < shMemorySize; ++i) {
                printf("[%d (%d)] \n", orders[i].size, orders[i].status);
            }
            printf("\n");
            printf("\n");
        }

        if (orders[orders[0].size].status == 0) {
            workerID++;
            workerID %= workers;
            sem_t * workerSemaphore = semaphoresIDs[workerID];
            if (getValueFromSemaphore(workerSemaphore) == 0) {
                sem_post(workerSemaphore);
            }
        }
        else {
            do {
                workerID++;
                workerID %= workers;
            } while (isIn(makers, MAKERS, workerID));
            sem_t * workerSemaphore = semaphoresIDs[workerID];
            if (getValueFromSemaphore(workerSemaphore) == 0) {
                sem_post(workerSemaphore);
            }
        }
        sleep(1);
    }
}


