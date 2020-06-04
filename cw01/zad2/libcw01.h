#ifndef LIB_CW01_H
#define LIB_CW01_H

struct BlockOfOperations
{
    int size;
    char ** operations;
};

struct ArrayOfBlocks {
    struct BlockOfOperations ** blocks;
    int size; 
};

struct ArrayOfBlocks *createMainArray(int size);
int comparePairAndWriteToFile(char *file1, char *file2);
void comparePairs(struct ArrayOfBlocks *mainArray, char *filesSequence);
void comparePair(struct ArrayOfBlocks *mainArray, char *pairOfFiles);
int addBlockOfOperations(struct ArrayOfBlocks *mainArray);
struct BlockOfOperations *createNewBlock();
int getCountOfOperationInBlock(struct ArrayOfBlocks * mainArray, int index);
void removeOperation(struct ArrayOfBlocks * mainArray, int blockIndex, int operationIndex);
void removeBlockOfOperation(struct ArrayOfBlocks *mainArray, int blockIndex);
void removeArrayOfBlocks(struct ArrayOfBlocks *mainArray);

#endif
