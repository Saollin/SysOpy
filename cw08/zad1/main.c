#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>

#include "operations.h"

int **imageArray;
int rows, columns;

pthread_t *threadsIDs;
int threadsNumber;

int semaphoreID;

const int pixelsNumber = 256;
int *result;

void createImageArray(char * imageFile) {
    FILE * image = fopen(imageFile, "r");
    char buff[32];
    fgets(buff, 32, image);
    int maxVal;
    fscanf(image, "%d%d%d", &columns, &rows, &maxVal);

    imageArray = (int**) calloc(rows, sizeof(int *));
    for(int i = 0; i < columns; i++) {
        imageArray[i] = (int *) calloc(columns, sizeof(int));
    }
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            fscanf(image, "%d", &imageArray[i][j]);
        }
    }

    fclose(image);
}

int getThreadIndex(pthread_t id) {
    for(int i = 0; i < threadsNumber; i++) {
        if(pthread_equal(id, threadsIDs[i])) {
            return i;
        }
    }
    return -1;
}

void * sign_mode_function() {
    struct timeval startTime, endTime;
    int threadId = getThreadIndex(pthread_self());
    
    int left = threadId * (pixelsNumber / threadsNumber + 1);
    int right = left + pixelsNumber / threadsNumber;

    gettimeofday(&startTime, NULL);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j <= columns; j++) {
            if(imageArray[i][j] >= left && imageArray[i][j] <= right) {
                result[imageArray[i][j]]++;
            }
        }
    }
    gettimeofday(&endTime, NULL);
    int timeValue = ((endTime.tv_sec - startTime.tv_sec) * 1e6) + (endTime.tv_usec - startTime.tv_usec);
    pthread_exit((void *) (intptr_t) timeValue);
}

void * block_mode_function() {
    struct timeval startTime, endTime;
    int threadId = getThreadIndex(pthread_self());
    
    int left = threadId * (int) ceil(columns / threadsNumber);
    int right = (threadId + 1) * (int) ceil(columns / threadsNumber) - 1;

    gettimeofday(&startTime, NULL);
    for(int i = 0; i < rows; i++) {
        for(int j = left; j <= right; j++) {
            semaphoreDecrease(semaphoreID);
            result[imageArray[i][j]]++;
            semaphoreIncrease(semaphoreID);
        }
    }
    gettimeofday(&endTime, NULL);
    int timeValue = ((endTime.tv_sec - startTime.tv_sec) * 1e6) + (endTime.tv_usec - startTime.tv_usec);
    pthread_exit((void *) (intptr_t) timeValue);
}

void * interleaved_mode_function() {
    struct timeval startTime, endTime;
    int threadId = getThreadIndex(pthread_self());
    int i = threadId;
    gettimeofday(&startTime, NULL);
    while(i < columns) {
        for(int j = 0; j < rows; j++) {
            semaphoreDecrease(semaphoreID);
            result[imageArray[i][j]]++;
            semaphoreIncrease(semaphoreID);
        }
        i += threadsNumber;
    }
    gettimeofday(&endTime, NULL);
    int timeValue = ((endTime.tv_sec - startTime.tv_sec) * 1e6) + (endTime.tv_usec - startTime.tv_usec);
    pthread_exit((void *) (intptr_t) timeValue);
}

void saveToFile(char * resultFile) {
    FILE * rFile = fopen(resultFile, "w");
    fwrite("P2\n", 3, 1, rFile);
    
    char * buf  = calloc(20, sizeof(char));
    sprintf(buf, "%d %d\n", 2, pixelsNumber);
    fwrite(buf, strlen(buf), 1, rFile);
    
    int maxNumber = 0;
    for (int i = 0; i < pixelsNumber; i++) {
        if(result[i] > maxNumber) {
            maxNumber = result[i];
        }
    }
    sprintf(buf, "%d\n", maxNumber);
    fwrite(buf, strlen(buf), 1, rFile);

    for(int i = 0; i < pixelsNumber; i++) {
        sprintf(buf, "%d %d\n", i, result[i]);
        fwrite(buf, strlen(buf), 1, rFile);
    }

    fclose(rFile);
}

void printTime(int microseconds) {
    int seconds = microseconds / 1e6;
    microseconds -= seconds * 1e6;
    int miliseconds = microseconds / 1e3;
    microseconds -= miliseconds * 1e3;
    printf("operating time: %d seconds %d milliseconds %d microseconds\n", seconds, miliseconds, microseconds);
}

int main(int argc, char ** argv) {
    if(argc != 5) {
        fprintf(stderr, "Wrong number of arguments!");
        return -1;
    }

    threadsNumber = atoi(argv[1]);
    threadsIDs = (pthread_t *)calloc(threadsNumber, sizeof(pthread_t));

    result = (int *)calloc(pixelsNumber, sizeof(int));
    for(int i = 0; i < pixelsNumber; i++) {
        result[i] = 0;
    }

    createImageArray(argv[3]);

    char * resultFile = argv[4];

    semaphoreID = createSemaphore();

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    
    for (int i = 0; i < threadsNumber; i++) {
        if (!strcmp(argv[2],"sign")) {
            pthread_create(&threadsIDs[i], NULL, sign_mode_function, NULL);
        }
        else if (!strcmp(argv[2],"block")) {
            pthread_create(&threadsIDs[i], NULL, block_mode_function, NULL);
        }
        else if (!strcmp(argv[2],"interleaved")) {
            pthread_create(&threadsIDs[i], NULL, interleaved_mode_function, NULL);
        }
        else {
            fprintf(stderr, "%s is wrong type argument", argv[2]);
            return -1;
        }
    }

    for (int i = 0; i < threadsNumber; i++) {
        int timeValue;
        pthread_join(threadsIDs[i], (void**)&timeValue);
        printf("Thread number %d - ", i);
        printTime(timeValue);
    }

    saveToFile(resultFile);
    gettimeofday(&endTime, NULL);
    int timeValue = ((endTime.tv_sec-startTime.tv_sec) * 1e6) + (endTime.tv_usec-startTime.tv_usec);

    printf("Main program - ");
    printTime(timeValue);
    printf("\n\n");

    free(imageArray);
    free(threadsIDs);
    free(result);
    deleteSemaphore(semaphoreID);

    return 0;
}
