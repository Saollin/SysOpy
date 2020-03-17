#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>

void error(char *message) {
    perror(message);
    exit(-1);
}

void generate(char * fileName, int numOfRecords, int recordSize) {
    int size = 100;
    char command[size];

    snprintf(command, sizeof command, "head -c 10000000 /dev/urandom | tr -dc 'A-za-z' | fold -w %d | head -n %d > %s", recordSize, numOfRecords, fileName);
    int status = system(command);
    if (status != 0) {
        error("Error while generating");
    }
}

void sysCopy(char * fileFrom, char * fileTo, int numOfRecords, int recordSize) {
    int bufforSize = recordSize + 1;
    int from = open(fileFrom, O_RDONLY); 
    if (from < 0) {
        error("Cant open source file with sys functions");
    }
    int to = open(fileTo, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (to < 0) {
        error("Cant open destination file with sys functions");
    }
    char * buffor = calloc(bufforSize, sizeof(char));
    for(int i = 0; i < numOfRecords; i++) {
        int bites = read(from, buffor, bufforSize);
        if(bites != bufforSize || bites == 0) {
            error("Cant read from source file with sys functions");
        }
        if(write(to, buffor, bufforSize) != bufforSize) {
            error("Cant write to destination file with sys functions");
        }
    }
    free(buffor);
    close(from);
    close(to);
}

void libCopy(char * fileFrom, char * fileTo, int numOfRecords, int recordSize) {
    int bufforSize = recordSize + 1;
    FILE from = fopen(fileFrom, "r"); 
    if (from < 0) {
        error("Cant open source file to copy with lib functions");
    }
    FILE to = fopen(fileTo, "w");
    if (to < 0) {
        error("Cant open destination file with lib functions");
    }
    char * buffor = calloc(bufforSize, sizeof(char));
    for(int i = 0; i < numOfRecords; i++) {
        int bites = fread(buffor, sizeof(char), bufforSize, from);
        if( != bufforSize){
            error("Cant read from source file with lib functions");
        }
        if(write(buffor, sizeof(char), bufforSize, to) != bufforSize) {
            error("Cant write to destination file with lib functions");
        }
    }
    free(buffor);
    close(from);
    close(to);
}

void sysSort(char * fileName, int numOfRecords, int recordSize) {
    int file = open(fileName, O_RDWR);
    if (file < 0) {
        error("Cant open file to sort with sys functions");
    }
    sysQuickSort(f, numOfRecords, numOfRecords, 0, numOfRecords - 1);

    close(file);
}

void sysQuickSort(int file, int numOfRecords, int recordSize, int low, int high) {
    if(low < high) {
        int pivot = sysPartition(file, numOfRecords, recordSize, low, high);
        if(pivot != 0) {
            sysQuickSort(file, numOfRecords, recordSize, 0, pivot - 1);
        }
        if(pivot != numOfRecords) {
            sysQuickSort(file, numOfRecords, recordSize, pivot + 1, high);
        }
    }
}

int sysPartition(int file, int numOfRecords, int recordSize, int low, int high) {
    int bufforSize = recordSize + 1;
    char *buff1 = malloc(bufforSize * sizeof(char));
    char *buff2 = malloc(bufforSize * sizeof(char));
    if(lseek(file, high * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to lib sort");
    }
    if(read(file, buff1, bufforSize) < 0) {
        error("Cant read file to sys sort")
    }
    unsigned char minChar = buff1[0];
    int i = low -1;

    for(int j = low; j < high; j++) {
        if(lseek(file, j * bufforSize, SEEK_SET) < 0) {
            error("Cant seek file to lib sort");
        }
        if(read(file, buff2, bufforSize) < 0) {
            error("Cant read file to sys sort")
        }
        if(buff2[0] < minChar) {
            i++;
            sysSwapInFile(file, numOfRecords, recordSize, i, j);
        }
    }
    sysSwapInFile(file, numOfRecords, recordSize, i + 1, high);
    free(buff1);
    free(buff2);
    return (i + 1);
}

void sysSwapInFile(int file, int numOfRecords, int recordSize, int i, int j) {
    int bufforSize = recordSize + 1;
    char *buff1 = malloc(bufforSize * sizeof(char));
    char *buff2 = malloc(bufforSize * sizeof(char));
    if(lseek(file, i * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 1");
    }
    if(read(file, buff1, bufforSize) < 0) {
        error("Cant read file to swap records 2")
    }
    if(lseek(file, j * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 3");
    }
    if(read(file, buff2, bufforSize) < 0) {
        error("Cant read file to swap records 4")
    }
    if(lseek(file, i * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 5");
    }
    if(write(file, buff2, bufforSize) < 0) {
        error("Cant write in file to swap records 6")
    }
    if(lseek(file, j * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 7");
    }
    if(write(file, buff1, bufforSize) < 0) {
        error("Cant write in file to swap records 8")
    }
    free(buff1);
    free(buff2);
}

void libSort(char * fileName, int numOfRecords, int recordSize) {
    FILE *file = Fopen(fileName, "r+");
    if (file == NULL) {
        error("Cant open file to sort with lib functions");
    }
    libQuickSort(file, numOfRecords, numOfRecords, 0, numOfRecords - 1);

    close(file);
}

void libQuickSort(FILE * file, int numOfRecords, int recordSize, int low, int high) {
    if(low < high) {
        int pivot = libPartition(file, numOfRecords, recordSize, low, high);
        if(pivot != 0) {
            libQuickSort(file, numOfRecords, recordSize, 0, pivot - 1);
        }
        if(pivot != numOfRecords) {
            libQuickSort(file, numOfRecords, recordSize, pivot + 1, high);
        }
    }
}

int lilbPartition(FILE * file, int numOfRecords, int recordSize, int low, int high) {
    int bufforSize = recordSize + 1;
    char *buff1 = malloc(bufforSize * sizeof(char));
    char *buff2 = malloc(bufforSize * sizeof(char));
    if(fseek(file, high * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to lib sort");
    }
    if(fread(buff1, sizeof(char), bufforSize, file) < 0) {
        error("Cant read file to sys sort")
    }
    unsigned char minChar = buff1[0];
    int i = low - 1;

    for(int j = low; j < high; j++) {
        if(fseek(file, j * bufforSize, SEEK_SET) < 0) {
            error("Cant seek file to lib sort");
        }
        if(fread(buff2, sizeof(char), bufforSize, file) < 0) {
            error("Cant read file to sys sort")
        }
        if(buff2[0] < minChar) {
            i++;
            sysSwapInFile(file, numOfRecords, recordSize, i, j);
        }
    }
    libSwapInFile(file, numOfRecords, recordSize, i + 1, high);
    free(buff1);
    free(buff2);
    return (i + 1);
}

void libSwapInFile(FILE * file, int numOfRecords, int recordSize, int i, int j) {
    int bufforSize = recordSize + 1;
    char *buff1 = malloc(bufforSize * sizeof(char));
    char *buff2 = malloc(bufforSize * sizeof(char));
    if(fseek(file, i * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 1");
    }
    if(fread(buff1, sizeof(char), bufforSize, file) < 0) {
        error("Cant read file to swap records 2")
    }
    if(fseek(file, j * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 3");
    }
    if(fread(buff2, sizeof(char), bufforSize, file) < 0) {
        error("Cant read file to swap records 4")
    }
    if(fseek(file, i * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 5");
    }
    if(fwrite(buff2, sizeof(char), bufforSize, file) < 0) {
        error("Cant write in file to swap records 6")
    }
    if(fseek(file, j * bufforSize, SEEK_SET) < 0) {
        error("Cant seek file to swap records 7");
    }
    if(fwrite(buff1, sizeof(char), bufforSize, file) < 0) {
        error("Cant write in file to swap records 8")
    }
    free(buff1);
    free(buff2);
}