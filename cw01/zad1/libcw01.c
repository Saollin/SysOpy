#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "libcw01.h"

char *resultFile = "result.txt";

struct ArrayOfBlocks *createMainArray(int size){
    if(size < 0) {
        return NULL;
    }

    struct ArrayOfBlocks *array = (struct ArrayOfBlocks *) calloc(1, sizeof(struct ArrayOfBlocks));
    if (array == NULL) {
        fprintf(stderr, "Can't allocate main array of blocks\n");
    }

    array->size = size;
    array->blocks = (struct BlockOfOperations **) calloc(size, sizeof(struct BlockOfOperations *));

    return array;
}

int comparePairAndWriteToFile(char *file1, char *file2) {
    int sizeOfCommand = strlen(file1) + strlen(file2) + strlen(resultFile) + 10;
    char command[sizeOfCommand];
    int exitCode;

    exitCode = snprintf(command, sizeOfCommand, "diff %s %s > %s", file1, file2, resultFile);

    if(exitCode < 0) {
        fprintf(stderr, "Error ocurred while trying to create construct proper command\n");
        return 1;
    }

    int findStatus = system(command);
    if(findStatus == -1) {
        return findStatus;
    }

    return 0;
}

void comparePairs(struct ArrayOfBlocks *mainArray, char *filesSequence) {
    char *sequenceCopy = strdup(filesSequence);

    char *token = strtok(sequenceCopy, " ");
    int counter = 0;
    //count how many pairs has sequence got
    while(token != NULL) {
        counter++;
        token = strtok(NULL, " ");
    }
    char *pairs[counter];
    counter = 0;

    char *sequenceCopy2 = strdup(filesSequence);
    char *token2 = strtok(sequenceCopy2, " ");
    while(token2 != NULL) {
        pairs[counter] = token2;
        token2 = strtok(NULL, " ");
        counter++;
    }
    for(int i = 0; i < counter; i++) {
        comparePair(mainArray, pairs[i]);
    }
}

void comparePair(struct ArrayOfBlocks *mainArray, char *pairOfFiles) {
    char *pairCopy = strdup(pairOfFiles);
    char *file1 = strtok(pairCopy, ":");
    char *file2 = strtok(NULL, "");

    if(comparePairAndWriteToFile(file1, file2) != 0) {
        return;
    } else {
        addBlockOfOperations(mainArray);
    }
}

int addBlockOfOperations(struct ArrayOfBlocks *mainArray) {
    struct BlockOfOperations * block = createNewBlock(resultFile);
    if(block == NULL) {
        return -1;
    }

    for(int i = 0; i < mainArray->size; i++) {
        if(mainArray->blocks[i] == NULL) {
            mainArray->blocks[i] = block;
            return i;
        }
    }
    fprintf(stderr, "There is no more space in array\n");
    return -1;
}

struct BlockOfOperations *createNewBlock() {
    FILE *file = fopen(resultFile, "r");
    if(!file) {
        fprintf(stderr, "Problem with opening a file: %s\n", resultFile);
        return NULL;
    }

    struct BlockOfOperations * newBlock = (struct BlockOfOperations *) calloc(1, sizeof(struct BlockOfOperations));
    newBlock->size = 0;
    newBlock->operations = NULL;

    char *lineBuf = NULL;
    size_t lineBufSize = 0;
    char *operation = (char *) malloc(0);
    size_t operationBuforSize = 0;
    size_t oneOperationLength;
    size_t lineLenght;

    while(getline(&lineBuf, &lineBufSize, file) >= 0) {
        //new block always stars with digit
        if(isdigit(lineBuf[0])){ 
            for(int i = 0; i < strlen(operation); i++) {
                operation[i] = '\0';
            }
            oneOperationLength = 0;
            lineLenght = strlen(lineBuf);
            oneOperationLength = lineLenght + 1;
            if(operationBuforSize == 0) {
                operationBuforSize = oneOperationLength; //lenght of first line of file
                operation = (char *) realloc(operation, oneOperationLength * sizeof(char));
            }
            strcpy(operation, lineBuf); 
            if(newBlock->operations == NULL) {
                newBlock->operations = (char **) calloc(1, sizeof(char*));
            } else {
                newBlock->operations = (char **) realloc(newBlock->operations, (newBlock->size + 1) * sizeof(char *));
            }
            // int index = newBlock->size;
            newBlock->operations[newBlock->size] = (char *) calloc(oneOperationLength, sizeof(char));
            strcpy(newBlock->operations[newBlock->size], operation);
            newBlock->size++; 
        } else { //continuation of previous block
            lineLenght = strlen(lineBuf);
            oneOperationLength += lineLenght;
            if (oneOperationLength > operationBuforSize) {
                operation = realloc(operation, oneOperationLength * sizeof(char));
                operationBuforSize = oneOperationLength;
            }
            strcat(operation, lineBuf);
            char * currentOperation = newBlock->operations[newBlock->size - 1];
            currentOperation = (char *) realloc(currentOperation, oneOperationLength * sizeof(char));
            strcpy(currentOperation, operation);
            newBlock->operations[newBlock->size - 1] = currentOperation;
        }
    } 

    free(lineBuf);
    free(operation);
    fclose(file);

    return newBlock;
}

int getCountOfOperationInBlock(struct ArrayOfBlocks * mainArray, int index) {
    if(!mainArray || index > mainArray->size - 1 || !mainArray->blocks[index]) {
        return -1;
    }
    return mainArray->blocks[index]->size;
}

void removeOperation(struct ArrayOfBlocks * mainArray, int blockIndex, int operationIndex) {
    if (!mainArray) {
        fprintf(stderr, "Main array doesn't exist\n");
        return;
    }
    if (blockIndex > mainArray->size - 1 || !mainArray->blocks[blockIndex] ) {
        fprintf(stderr, "Wrong index of block - too big or block doesn't exist\n");
        return;
    }
    if (operationIndex > getCountOfOperationInBlock(mainArray, blockIndex)) {
        fprintf(stderr, "Wrong index of operation - too big\n");
        return;
    }
    if (!mainArray->blocks[blockIndex]->operations[operationIndex]) {
        fprintf(stderr, "Wrong index of operation - block doesn't exist\n");
        return;
    }
    free(mainArray->blocks[blockIndex]->operations[operationIndex]);
    mainArray->blocks[blockIndex]->operations[operationIndex] = NULL;
}

void removeBlockOfOperation(struct ArrayOfBlocks *mainArray, int blockIndex) {
    if (!mainArray) {
        fprintf(stderr, "Main array doesn't exist\n");
        return;
    }
    if (blockIndex > mainArray->size - 1) {
        fprintf(stderr, "Wrong index of block - too big\n");
        return;
    }
    if (!mainArray->blocks[blockIndex] ) {
        fprintf(stderr, "Wrong index of block - block doesn't exist\n");
        return;
    }
    for (int i = 0; i < getCountOfOperationInBlock(mainArray, blockIndex); i++) {
        if(mainArray->blocks[blockIndex]->operations[i] != NULL) { 
            removeOperation(mainArray, blockIndex, i);
        }
    }
    free(mainArray->blocks[blockIndex]);
    mainArray->blocks[blockIndex] = NULL;
}

void removeArrayOfBlocks(struct ArrayOfBlocks *mainArray) {
    for(int i = 0; i < mainArray->size; i++) {
        if(mainArray->blocks[i] != NULL) { 
            removeBlockOfOperation(mainArray,i);
        }
    }
    free(mainArray->blocks);
    free(mainArray);
}


