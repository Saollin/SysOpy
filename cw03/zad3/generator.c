#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>

int minGlob,maxGlob; 

void countRightInt(int *n, int min, int max) {
    if(*n == INT_MIN) {
        *n = *n + 1;
    }

    *n = abs(*n);
    *n = ((*n) % (max - min + 1)) + min;
}

void writeToFile(char * filePath, int col, int row) {
    FILE * file = fopen(filePath, "w");
    int randInt;

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            randInt = rand();
            countRightInt(&randInt, -100, 100);
            fprintf(file, "%d", randInt);
            if(j == col - 1) {
                fprintf(file, "\n");
            }
            else {
                fprintf(file, " ");
            }
        }
    }
    fclose(file);
}
int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Too few parameters provided!\nPlease provide parameters like that:\n./generator [number] [min] [max]\n");
        return -1;
    }
    int numberOfPairs = atoi(argv[1]);
    int minGlob = atoi(argv[2]);
    int maxGlob = atoi(argv[3]);
    
    system("touch list.txt");
    FILE *list = fopen("list.txt","w");
    if (minGlob > maxGlob) { //switch if min max are in wrong order
        int tmp = minGlob;
        minGlob = maxGlob;
        maxGlob = tmp;
    }

    time_t t;
    srand((unsigned) time(&t));
    for(int i = 0; i < numberOfPairs; i++) {
        int col1, row1, col2; //row1 = col2
        col1 = rand();
        row1 = rand();
        col2 = rand();
        countRightInt(&col1, minGlob, maxGlob);
        countRightInt(&row1, minGlob, maxGlob);
        countRightInt(&col2, minGlob, maxGlob);
        
        char fileA[20], fileB[20], resultFile[20];
        sprintf(fileA, "m%d_A.txt", i + 1);
        sprintf(fileB, "m%d_B.txt", i + 1);
        sprintf(resultFile, "m%d_result.txt", i + 1);

        char command[30];
        sprintf(command, "touch %s", resultFile); // creating resultFile
        system(command);
        //save in argument file
        char fullLine[60];
        sprintf(fullLine, "%s %s %s\n", fileA, fileB, resultFile);
        fwrite(fullLine, sizeof(char), strlen(fullLine), list);

        writeToFile(fileA, col1, row1);
        writeToFile(fileB, col2, col1);
    }
    fclose(list);
    return 0;
}