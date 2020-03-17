#ifndef DLL
#include "libcw01.h"
#else
#include "cw01_dll.h"
#endif

#include <dlfcn.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

clock_t startTime, endTime;
struct tms startTms, endTms;
struct ArrayOfBlocks *mainArray;

void start() {
    startTime = times(&startTms);
}

void parseCreatingTable(char *argv[], int i, int argc) {
    if(i + 1 >= argc) {
        fprintf(stderr, "Wrong number of arguments in create_table");
        return;
    }
    int size = atoi(argv[i +1]);
    mainArray = createMainArray(size);
}

void parseCompairingPairs(char *argv[], int i, int argc) {
    if(i + 1 >= argc) {
        fprintf(stderr, "Wrong number of argument in compare_pairs");
        return;
    }
    comparePairs(mainArray, argv[i + 1]);
}

void parseRemovingBlock(char *argv[], int i, int argc) {
    if(i + 1 >= argc) {
        fprintf(stderr, "Wrong number of argument in compare_pairs");
        return;
    }
    int blockIndex = atoi(argv[i + 1]);
    removeBlockOfOperation(mainArray, blockIndex);
}

void parseRemovingOperation(char *argv[], int i, int argc) {
    if(i + 2 >= argc) {
        fprintf(stderr, "Wrong number of argument in compare_pairs");
        return;
    }
    int blockIndex = atoi(argv[i + 1]);
    int operationIndex = atoi(argv[i + 2]);
    // printf("%d %d\n", blockIndex, operationIndex);
    // printf("%s\n", mainArray->blocks[0]->operations[0]);
    removeOperation(mainArray,blockIndex,operationIndex);
}

void end(char *nameOfOperation) {
    endTime = times(&endTms);
    int tics = sysconf(_SC_CLK_TCK);
    double realTime = ((double)(endTime - startTime)) / tics;
    double userTime = (double)(endTms.tms_utime - startTms.tms_utime) / tics;
    double systemTime = (double)(endTms.tms_stime - startTms.tms_stime) / tics;
    double childUserTime = (double)(endTms.tms_cutime - startTms.tms_cutime) / tics;
    double childSystemTime = (double)(endTms.tms_cstime - startTms.tms_cstime) / tics;
    printf("%s\n", nameOfOperation);
    printf("%20s\t%20s\t%20s\t%20s\t%20s\n",
    "Real [s]",
    "User [s]",
    "System [s]",
    "Child User [s]",
    "Child System [s]");
    printf("%20f\t%20f\t%20f\t%20f\t%20f\n", 
    realTime,
    userTime,
    systemTime,
    childUserTime,
    childSystemTime);
}

int main(int argc, char *argv[]) {
#ifdef DLL
    initLibrary();
#endif
    int i = 1;
    while(i < argc) {
        if(!strcmp(argv[i], "start")) {
            start();
            i += 1;
        }
        if(!strcmp(argv[i], "create_table")) {
            parseCreatingTable(argv, i, argc);
            i += 2;
        }
        else if(!strcmp(argv[i], "compare_pairs")) {
            parseCompairingPairs(argv, i, argc);
            i += 2;
        }
        else if(!strcmp(argv[i], "remove_block")) {
            parseRemovingBlock(argv, i, argc);
            i += 2;
        }
        else if(!strcmp(argv[i], "remove_operation")) {
            parseRemovingOperation(argv, i, argc);
            i += 3;
        }
        else if(!strcmp(argv[i], "end")) {
            end(argv[i + 1]);
            i += 2;
        }
        else {
            fprintf(stderr, "Wrong command\n");
            return -1;
        }
    }
    printf("\n\n\n");
    removeArrayOfBlocks(mainArray);

#ifdef DLL
  dlclose(handle);
#endif
    return 0;
}