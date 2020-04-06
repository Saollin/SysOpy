#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char ** argv) {
    char *consumer[6] = {"./consumer", "fifo", "./tests/result.txt", "20", NULL};

    char ***producer = calloc(5, sizeof(char **));
    for(int i = 0; i < 5; i++) {
        char * fileName = calloc(14, sizeof(char));
        sprintf(fileName, "./tests/p%d.txt", i + 1);
        producer[i] = calloc(5, sizeof(char *));
        producer[i][0] ="./producer";
        producer[i][1] = "fifo"; 
        producer[i][2] = fileName;
        producer[i][3] =  "3";
        producer[i][4] = NULL;
    }
    
    
    mkfifo("fifo", S_IRUSR | S_IWUSR);

    pid_t pids[6];

    for(int i = 0; i < 5; i++) {
        if((pids[i] = fork()) == 0) {
            execvp(producer[i][0], producer[i]);
        }
    }

    if((pids[5] == fork()) == 0) {
        execvp(consumer[0], consumer);
    }

    for(int i = 0; i < 6; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}   