#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h> 

int main(int argc, char ** argv) {
    if (argc < 4) {
        fprintf(stderr, "Wrong number of arguments! Usage of program: \n ./producer [fifo] [file] [number of chars]");
        return -1;
    }

    char * fifoPath = argv[1];
    FILE * fifo;
    if((fifo = fopen(fifoPath, "w")) == NULL) {
        fprintf(stderr, "Consumer have failed to open file!");
        return -1;
    }

    char * fileName = argv[2];
    FILE * file;
    if((file = fopen(fileName, "r+")) == NULL) {
        fprintf(stderr, "Consumer have failed to open file!");
        return -1;
    }

    int numberOfChars = atoi(argv[3]) + 1; //fgets add \0
    char fromFile[numberOfChars];

    pid_t processPid = getpid();

    srand((unsigned) time(NULL));
    int sleepTime;
    while (fgets(fromFile, numberOfChars, file) != NULL)
    {
        fprintf(fifo, "#%d#%s\n", processPid, fromFile);
        printf("#%d#%s\n", processPid, fromFile);
        sleepTime = rand() % 3;
        sleep(sleepTime);
    }

    fclose(fifo);
    fclose(file);

    return 0;
}   