#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int numberOfPairs;


void checkMatrixesInPair(int pair, FILE * list) {
    //parsing
    int **A;
    int **B;
    int **Result;
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
    int rows2 = 0;
    int columns2 = 1;
    while (fgets(buf,sizeof(buf),a) != NULL) rows1++;
    while (fgets(buf,sizeof(buf),b) != NULL) rows2++;
    fseek(a,0,SEEK_SET);
    fseek(b,0,SEEK_SET);
    fseek(result, 0, SEEK_SET);
    fgets(buf,sizeof(buf),b);
    for(int j=0; j<strlen(buf); j++)
        if(buf[j] == ' ') columns2++;
    fseek(b,0,SEEK_SET);
    int check1 = 0;
    int check2 = 0;
    while (fgets(buf,sizeof(buf),result) != NULL) check1++;
    for(int j=0; j<strlen(buf); j++)
        if(buf[j] == ' ') check2++;
    if(check1 != rows1 || check2 != columns2) {
        printf("The multiplication didn't manage to end in given time! Give it more time please!\n");
        return;
    }
    fseek(result,0,SEEK_SET);
    A = (int **)calloc(rows1,sizeof(int *));
    B = (int **)calloc(rows2,sizeof(int *));
    Result = (int **)calloc(rows1,sizeof(int *));
    for(int j=0; j<rows1; j++) {
        A[j] = (int *)calloc(rows2,sizeof(int));
        Result[j] = (int *)calloc(columns2,sizeof(int));
    }
    for(int j=0; j<rows2; j++) B[j] = (int *)calloc(columns2,sizeof(int));
    int j = 0;
    while(fgets(buf,sizeof(buf),a) != NULL) {
        int c;
        for(int k=0; k<rows2; k++) {
            if (k == 0) c = atoi(strtok(buf," "));
            else if(k == rows2-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            A[j][k] = c;
        }
        j++;
    }
    j=0;
    while(fgets(buf,sizeof(buf),b) != NULL) {
        int c;
        for(int k=0; k<columns2; k++) {
            if (k == 0) c = atoi(strtok(buf," "));
            else if(k == columns2-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            B[j][k] = c;
        }
        j++;
    }
    j=0;
    while(fgets(buf,sizeof(buf), result) != NULL) {
        char *buf2 = buf;
        while(isspace(*buf2)) buf2++;
        int c;
        for(int k=0; k<columns2; k++) {
            if(k == 0) c = atoi(strtok(buf," "));
            else if (k == columns2-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            Result[j][k] = c;
        }
        j++;
    }
    fclose(a);
    fclose(b);
    fclose(result);

    /* checking */
    int error = 0;
    for (int j=0; j<rows1; j++) {
        for(int k=0; k<columns2; k++) {
            int res = 0;
            for (int l=0; l<rows2; l++) 
                res += A[j][l]*B[l][k];
            if (res != Result[j][k]) {
                error = 1;
                break;
            }
        }
        if(error == 1) break;
    }
    if (error == 1) printf("This multiplication of pair no %d is not correct!\n",pair);
    else printf("This multiplication of pair no %d on the list went correctly!\n", pair);
}


int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("WRONG NUMBER OF PARAMETERS!\n");
        return -1;
    }
    char *fileName = argv[1];
    FILE *list = fopen(fileName,"r");
    numberOfPairs=atoi(argv[2]);

    for(int i=0; i<numberOfPairs; i++) {
        checkMatrixesInPair(i+1, list);
    }
    printf("\n\n\n");
    fclose(list);
    return 0;
}