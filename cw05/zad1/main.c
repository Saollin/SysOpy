#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const int maxNumberOfPrograms = 16;
const int maxNumberOfArguments = 8;

int parsePrograms(char * line, char ** programs) {
    char * program;
    int i = 0;
    for(program = strtok(line, "|"); i < maxNumberOfPrograms && program; i++) {
        programs[i] = calloc(strlen(program), sizeof(char));
        strcpy(programs[i], program);
        program = strtok(NULL, "|");
    }
    return i;
}

// ja jak będę mieć czas to chciałem telefony poprzeglądać, bo chyba będę nowy kupować
void execute(char * line) {
    
    char ** programs = calloc(maxNumberOfPrograms, sizeof(char *));
    int numberOfPrograms = parsePrograms(line, programs);

    int old[2];
    int new[2];

    pid_t pids[numberOfPrograms];

    for(int i = 0; i < numberOfPrograms; i++) {
        char ** args = calloc(maxNumberOfArguments + 1, sizeof(char *));
        
        char * word = strtok(programs[i], " \n");
        //get name of program 
        char * name = calloc(strlen(word), sizeof(char));
        strcpy(name, word);
        
        //parsing args
        int j = 0;
        while ((word = strtok(NULL, " \n"))) {
            args[j+1] = calloc(strlen(word), sizeof(char)) ;   
            strcpy(args[j+1], word);
            j++;
        }
        args[0] = calloc(strlen(name), sizeof(char));
        strcpy(args[0], name);
        
        if(i < numberOfPrograms - 1 && pipe(new) < 0) {
            perror("Program have failed to make new pipe");
            return;
        }

        pid_t child = fork();
        
        if (child < 0) {
            perror("Program have failed to fork new process");
            return;
        } 
        else if (child == 0) {
            if (i < numberOfPrograms - 1) {
                dup2(new[1], STDOUT_FILENO);
                close(new[0]);
                close(new[1]);
            }
            if (i > 0) {
                dup2(old[0], STDIN_FILENO);
                close(old[1]);
                close(old[0]);
            }
            execvp(name, args);
            exit(0);
        }
        else {
            if (i > 0) {
                close(old[1]);
                close(old[0]);
            }
            if(i < numberOfPrograms - 1) {
                old[0] = new[0];
                old[1] = new[1];
            }
            pids[i] = child;
        }
    }
    for (int i = 0; i < numberOfPrograms; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}

int main(int argc, char ** argv) {
    if (argc <  2) {
        fprintf(stderr, "Wrong number of arguments!");
        return -1;
    }

    char * fileName = argv[1];
    FILE *file = fopen(fileName, "r");
    if(file == NULL) {
        fprintf(stderr, "Program failed to open file %s\n", fileName);
        return -1;
    }

    char buff[200];
    char * line;
    while((line = fgets(buff,sizeof(buff), file)) != NULL) {   
        printf("\nLine: %s\n", line);  
        execute(line);
    }

    fclose(file);
    return 0;
}
