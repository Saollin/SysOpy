#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int numberOfPairs;

void parseMatrix(int **A, FILE * matrixFile, int colNumber) {
    char buf[500];
    int j = 0;
    fseek(matrixFile, 0, SEEK_SET);
    while(fgets(buf,sizeof(buf),matrixFile) != NULL) {
        int c;
        for(int k=0; k<colNumber; k++) {
            if (k == 0) c = atoi(strtok(buf," "));
            else if(k == colNumber-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            A[j][k] = c;
        }
        j++;
    }
}


void countNumberOfPairs(FILE * list) {
    fseek(list,0,SEEK_SET);
    char buffer[500];
    numberOfPairs = 0;
    if(list != NULL) {
        while(fgets(buffer,sizeof(buffer), list) != NULL)
            numberOfPairs++;
    }
}

void checkMatrixesInPair(int pair, FILE * list) {
    //parsing
    int **matrixA;
    int **matrixB;
    int **matrixResult;
    int i = 0;
    char buf[500];
    fseek(list, 0, SEEK_SET);
    while(i++<pair) {
        fgets(buf,sizeof(buf), list);
    }
    FILE *a = fopen(strtok(buf," "),"r");
    FILE *b = fopen(strtok(NULL, " "),"r");
    FILE *result = fopen(strtok(NULL,"\n"),"r");
    if(a == NULL || b == NULL || result == NULL) {
        fprintf(stderr, "Can't open files with result and matrixes.\n");
        return;
    }
    int rows1 = 0;
    int rows2 = 0; //also columns1
    int columns2 = 1;
    while (fgets(buf,sizeof(buf),a) != NULL) {
        rows1++;
    }
    while (fgets(buf,sizeof(buf),b) != NULL) {
        rows2++;
    }
    fseek(b,0,SEEK_SET);
    fgets(buf,sizeof(buf),b);
    for(int j=0; j<strlen(buf); j++) {
        if(buf[j] == ' ') columns2++;
    }

    int resultRow = 0;
    int resultColumns = 0;
    fseek(result, 0, SEEK_SET);
    while (fgets(buf,sizeof(buf),result) != NULL) {
        resultRow++;
    }
    for(int j=0; j<strlen(buf); j++){
        if(buf[j] == ' ') {
            resultColumns++;
        }
    }
    if(resultRow != rows1 || resultColumns != columns2) {
        printf("The multiplication had too little time!\n");
        return;
    }
    
    matrixA = (int **)calloc(rows1,sizeof(int *));
    matrixB = (int **)calloc(rows2,sizeof(int *));
    matrixResult = (int **)calloc(rows1,sizeof(int *));
    for(int j=0; j<rows1; j++) {
        matrixA[j] = (int *)calloc(rows2,sizeof(int));
        matrixResult[j] = (int *)calloc(columns2,sizeof(int));
    }
    for(int j=0; j<rows2; j++) {
        matrixB[j] = (int *)calloc(columns2,sizeof(int));
    }

    parseMatrix(matrixA, a, rows2);
    parseMatrix(matrixB, b, columns2);
    parseMatrix(matrixResult, result, columns2);
    
    fclose(a);
    fclose(b);
    fclose(result);

    /* checking */
    int error = 0;
    for (int j=0; j<rows1; j++) {
        for(int k=0; k<columns2; k++) {
            int result = 0;
            for (int l=0; l<rows2; l++) 
                result += matrixA[j][l]*matrixB[l][k];
            if (result != matrixResult[j][k]) {
                error = 1;
                goto end;
            }
        }
    }
    end:
    if (error == 1) printf("The pair no %d was not counted correctly!\n",pair);
    else printf("The pair no %d was counted correctly!\n", pair);
}


int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    char *fileName = argv[1];
    FILE *list = fopen(fileName,"r");
    countNumberOfPairs(list);

    for(int i=0; i<numberOfPairs; i++) {
        checkMatrixesInPair(i+1, list);
    }
    printf("\n\n\n");
    fclose(list);
    return 0;
}