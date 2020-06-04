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
    FILE * fifo;
    if((fifo = fopen(fifoPath, "r")) == NULL) {
        fprintf(stderr, "Consumer have failed to open fifo!\n");
        return -1;
    }

    char * fileName = argv[2];
    FILE * file;
    if((file = fopen(fileName, "w")) == NULL) {
        fprintf(stderr, "Consumer have failed to open file: %s", fileName);
        return -1;
    }

    int numberOfChars = atoi(argv[3]);
    char fromFifo[numberOfChars];

    while (fgets(fromFifo, numberOfChars, fifo) != NULL)
    {
        fprintf(file, "%s", fromFifo);
    }

    fclose(fifo);
    fclose(file);
    return 0;
}   