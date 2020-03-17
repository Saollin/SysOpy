#ifndef CW01_DLL_H
#define CW01_DLL_H

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

struct BlockOfOperations
{
    int size;
    char ** operations;
};

struct ArrayOfBlocks {
    struct BlockOfOperations ** blocks;
    int size; 
};

static void *handle = NULL;

struct ArrayOfBlocks *(*_createMainArray)(int size);
int (*_comparePairAndWriteToFile)(char *file1, char *file2);
void (*_comparePairs)(struct ArrayOfBlocks *mainArray, char *filesSequence);
void (*_comparePair)(struct ArrayOfBlocks *mainArray, char *pairOfFiles);
int (*_addBlockOfOperations)(struct ArrayOfBlocks *mainArray);
struct BlockOfOperations *(*_createNewBlock)(void);
int (*_getCountOfOperationInBlock)(struct ArrayOfBlocks * mainArray, int index);
void (*_removeOperation)(struct ArrayOfBlocks * mainArray, int blockIndex, int operationIndex);
void (*_removeBlockOfOperation)(struct ArrayOfBlocks *mainArray, int blockIndex);
void (*_removeArrayOfBlocks)(struct ArrayOfBlocks *mainArray);

void initLibrary() {
    handle = dlopen("libcw01.so", RTLD_NOW);
    if (handle==NULL) {
        fprintf(stderr, "Problem with opening dynamic library");
        return;
    }
    _createMainArray = dlsym(handle, "createMainArray");
    _comparePairAndWriteToFile = dlsym(handle, "comparePairAndWriteToFile");
    _comparePairs = dlsym(handle, "comparePairs");
    _comparePair = dlsym(handle, "comparePair");
    _addBlockOfOperations = dlsym(handle, "addBlockOfOperations");
    _getCountOfOperationInBlock = dlsym(handle, "getCountOfOperationInBlock");
    _removeOperation = dlsym(handle, "removeOperation");
    _removeBlockOfOperation = dlsym(handle, "removeBlockOfOperation");
    _removeArrayOfBlocks = dlsym(handle, "removeArrayOfBlocks"); 
}

struct ArrayOfBlocks *createMainArray(int size) {
    return (*_createMainArray)(size);
}
int comparePairAndWriteToFile(char *file1, char *file2) {
    return (*_comparePairAndWriteToFile)(file1,file2);
}
void comparePairs(struct ArrayOfBlocks *mainArray, char *filesSequence) {
    return (*_comparePairs)(mainArray, filesSequence);
}
void comparePair(struct ArrayOfBlocks *mainArray, char *pairOfFiles) {
    return (*_comparePair)(mainArray, pairOfFiles);
}
int addBlockOfOperations(struct ArrayOfBlocks *mainArray) {
    return (*_addBlockOfOperations)(mainArray);
}
struct BlockOfOperations *createNewBlock(void) {
    return (*_createNewBlock)();
}
int getCountOfOperationInBlock(struct ArrayOfBlocks * mainArray, int index) {
    return (*_getCountOfOperationInBlock)(mainArray, index);
}
void removeOperation(struct ArrayOfBlocks * mainArray, int blockIndex, int operationIndex) {
    return (*_removeOperation)(mainArray, blockIndex, operationIndex);
}
void removeBlockOfOperation(struct ArrayOfBlocks *mainArray, int blockIndex) {
    return (*_removeBlockOfOperation)(mainArray, blockIndex);
}
void removeArrayOfBlocks(struct ArrayOfBlocks *mainArray) {
    return (*_removeArrayOfBlocks)(mainArray);
}

#endif