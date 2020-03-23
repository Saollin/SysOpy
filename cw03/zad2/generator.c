#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>

int minGlob,maxGlob; 

void correctInt(int *n, int min, int max) {
    if(*n == INT_MIN) {
        *n = *n + 1;
    }

    *n = abs(*n);
    *n = ((*n) % (max - min + 1)) + min;
}

void writeToFile(char * filePath, int col, int row, FILE * randomGenerator) {
    FILE * file = fopen(filePath, "w");
    int randInt;

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            fread(&randInt, sizeof(int), 1, randomGenerator);
            correctInt(&randInt, -100, 100);
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

    FILE *random = fopen("/dev/random", "r");
    if(random == NULL) {
        printf("Can't open /dev/random\n");
        exit(1);
    }
    for(int i = 0; i < numberOfPairs; i++) {
        int col1, row1, row2; //row1 = col2
        fread(&col1, sizeof(int), 1, random);
        fread(&row1, sizeof(int), 1, random); 
        fread(&row2, sizeof(int), 1, random); 
        correctInt(&col1, minGlob, maxGlob);
        correctInt(&row1, minGlob, maxGlob);
        correctInt(&row2, minGlob, maxGlob);
        
        char fileA[15], fileB[15], resultFile[15];
        sprintf(fileA, "m%d_A.txt", i + 1);
        sprintf(fileB, "m%d_B.txt", i + 1);
        sprintf(resultFile, "m%d_result.txt", i + 1);

        char command[30];
        sprintf(command, "touch %s", resultFile); // creating resultFile
        
        //save in argument file
        char fullLine[45];
        sprintf(fullLine, "%s %s %s\n", fileA, fileB, resultFile);
        fwrite(fullLine, sizeof(char), strlen(fullLine), list);

        writeToFile(fileA, col1, row1, random);
        writeToFile(fileB, row1, row2, random);
    }
    fclose(random);
    fclose(list);
    return 0;
}