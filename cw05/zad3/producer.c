#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv) {
    if (argc < 4) {
        fprintf(stderr, "Wrong number of arguments! Usage of program: \n ./producer [fifo] [file] [number of chars]");
        return -1;
    }

    char * fifoPath = argv[1];
    FILE * fifo = fopen(fifoPath, "w");

    char * fileName = argv[2];
    FILE * file = fopen(fileName, "r+");

    int numberOfChars = atoi(argv[3]);
    char fromFile[numberOfChars];
    char toFifo[numberOfChars];

    pid_t processPid = getpid();

    srand((unsigned) time(NULL));
    int sleepTime;
    while (fread(fromFile, sizeof(char), numberOfChars, file))
    {
        fprintf(fifo, "#%d#%s", processPid, fromFile);
        sleepTime = rand() % 3;
        sleep(sleepTime);
    }

    fclose(fifo);
    fclose(file);

    return 0;
}   