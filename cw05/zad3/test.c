#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "Wrong number of arguments! Give buffored chars");
        return -1;
    }

    int numberOfChars = atoi(argv[1]);
    char * chars = calloc(2, sizeof(char));
    sprintf(chars, "%d", numberOfChars + 10);

    char *consumer[] = {"./consumer", "fifo", "./tests/result.txt", chars, NULL};

    char ***producer = calloc(5, sizeof(char **));
    for(int i = 0; i < 5; i++) {
        char * fileName = calloc(14, sizeof(char));
        sprintf(fileName, "./tests/p%d.txt", i + 1);
        producer[i] = calloc(5, sizeof(char *));
        producer[i][0] ="./producer";
        producer[i][1] = "fifo"; 
        producer[i][2] = fileName;
        producer[i][3] =  argv[1];
        producer[i][4] = NULL;
    }
    
    
    mkfifo("fifo", S_IRUSR | S_IWUSR);

    pid_t pids[6];


    if ((pids[0] = fork()) == 0)
        execvp(producer[0][0], producer[0]);

    if ((pids[1] = fork()) == 0)
        execvp(producer[1][0], producer[1]);

    if ((pids[2] = fork()) == 0)
        execvp(producer[2][0], producer[2]);

    if ((pids[3] = fork()) == 0)
        execvp(producer[3][0], producer[3]);

    if ((pids[4] = fork()) == 0)
        execvp(producer[4][0], producer[4]);

    if((pids[5] == fork()) == 0) {
        execvp(consumer[0], consumer);
    }

    for(int i = 0; i < 6; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}   