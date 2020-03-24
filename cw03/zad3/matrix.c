
#define _POSIX_C_SOURCE 1
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h> 
#include <sys/wait.h>
#include <sys/resource.h>
#include <libgen.h>
#include <errno.h>


clock_t startTime, endTime;
struct tms startTms, endTms;
int maxSeconds;
int processesNumber;
int numberOfPairs;
int mode;

void error(char *message) {
    perror(message);
    exit(-1);
}

void start() {
    startTime = times(&startTms);
}

void end() {
    endTime = times(&endTms);
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

int countRowOfResultMatrix(FILE * a) {
    fseek(a, 0, SEEK_SET);
    char buffer[500];
    int resultRow = 0;
    while(fgets(buffer, sizeof(buffer), a) != NULL) {
        resultRow++;
    }
    return resultRow;
}

int countColumnOfResultMatrix(FILE * b) {
    fseek(b, 0, SEEK_SET);
    char buffer[500];
    int resultCol = 1;
    fgets(buffer, sizeof(buffer), b);
    for(int j=0; j<strlen(buffer); j++){
        if(buffer[j] == ' ') resultCol++;
    }
    return resultCol;
}

int processChild(int n, int mode, FILE * list) {
    start();
    int multiplications = 0;
    fseek(list, 0, SEEK_SET);
    char buf[500];
    int result, lineNumber;
    int resultCol, jump;
    int i = 0;
    if (list != NULL) {
        while(fgets(buf,sizeof(buf), list) != NULL && i<numberOfPairs) {
            char * aPath = strtok(buf," ");
            FILE *a = fopen(aPath,"r");
            FILE *b = fopen(strtok(NULL," "),"r");
            char *f = strtok(NULL,"\n");
            int s = open(f,O_WRONLY);
            flock(s,LOCK_EX);
            char rowBuf[500];
            fseek(a,0,SEEK_SET);
            end();
            if((double)(endTime-startTime)/sysconf(_SC_CLK_TCK) > maxSeconds)
                exit(multiplications);

            resultCol = countColumnOfResultMatrix(b);
            jump = resultCol / processesNumber;
            if(resultCol%processesNumber != 0) jump++;
            if (a != NULL) {
                lineNumber = 0;
                while(fgets(rowBuf,sizeof(rowBuf),a) != NULL) {
                    int k=0;
                    int j = n*jump;
                    while (k++ < jump) {
                        result = 0;
                        fseek(b,0,SEEK_SET);
                        int count = 0;
                        if (b != NULL) {
                            char buf2[500];
                            while(fgets(buf2,sizeof(buf2),b) != NULL) {
                                char copyRowBuf[500];
                                strcpy(copyRowBuf,rowBuf);
                                char *numberInColumn;
                                for(int m = 0; m<=j; m++) {
                                    if(m==0) numberInColumn = strtok(buf2," ");
                                    else numberInColumn = strtok(NULL," ");
                                }
                                char *numberInRow;
                                for(int n = 0; n<=count; n++) {
                                    if(n==0) numberInRow = strtok(copyRowBuf," ");
                                    else numberInRow = strtok(NULL," ");
                                }
                           
                                result += atoi(numberInColumn)*atoi(numberInRow);
                                count++;
                            }
                        }
                        if(mode == 1) {
                            char command[50];
                            sprintf(command,"sed -i '%ds/$/ %d/' %s",lineNumber+1,result, f);
                            system(command);
                        }
                        else {
                            char command[50];
                            sprintf(command,"echo -n '%d ' >> result_%d_%d",result,i,lineNumber);
                            system(command);
                        }
                        j++;
                        if(j == resultCol) break;
                    }
                    lineNumber++;    
                }
            }

            i++;
            multiplications++;
            fclose(a);
            fclose(b);
            flock(s,LOCK_UN);
            close(s);
        }
    }
    return multiplications;
}


void writeResultToOneFile(FILE * list) {
    int i = 0;
    char buf[500];
    fseek(list, 0, SEEK_SET);
    while(fgets(buf,sizeof(buf), list) != NULL && i<numberOfPairs) {
            FILE *a = fopen(strtok(buf," "),"r");
            int resultRow = countRowOfResultMatrix(a);
            char *arg[resultRow+5];
            for(int j=0; j<resultRow+4; j++) arg[j] = (char *)calloc(20,sizeof(char));
            arg[0]="paste";
            arg[1]="-d";
            arg[2]=" ";
            arg[3]="-s";
            char tmp[50];
            for(int j=0; j<resultRow; j++) {
                sprintf(tmp,"result_%d_%d",i,j);
                strcpy(arg[j+4],tmp);
            }
            arg[resultRow+4] = NULL;
            char resultFileName[15];
            sprintf(resultFileName,"m%d_result.txt",i+1);
            // pid_t child = vfork();
            if(vfork() == 0) {
                int fd = open(resultFileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(fd,1);  
                close(fd);
                execv("/usr/bin/paste",arg);
            }
            wait(NULL);
            i++;
    }
}

void parseMode(char * modeArgument, FILE * list) {
    char buf[500];
    FILE * a;
    int resultRow;
    fseek(list,0,SEEK_SET);
    char command[50];
    mode = atoi(modeArgument);
    if(mode == 1) {
        if (list != NULL)
            while(fgets(buf,sizeof(buf),list) != NULL) {
                a = fopen(strtok(buf," "), "r");
                resultRow = countRowOfResultMatrix(a);
                strtok(NULL," ");
                char *resultFile = strtok(NULL,"\n");
                sprintf(command,"echo '' >> %s", resultFile);
                for(int i=0; i<resultRow; i++)
                    system(command);
            }
    }
    else if(mode ==0) {
        int i = 0;
        if (list != NULL)
            while(fgets(buf,sizeof(buf),list) != NULL) {
                a = fopen(strtok(buf," "), "r");
                resultRow = countRowOfResultMatrix(a);
                for(int j=0; j<resultRow; j++) {
                    sprintf(command,"touch result_%d_%d",i,j);
                    system(command);
                }
                i++;
            }
    }
}


int main(int argc, char ** argv) {
    if (argc != 7) {
        printf("Wrong number of arguments! Usage of program is like this:\n./matrix [list] [number_of_processes] [max_time] [mode] [cpuTime] [virtualMemorySize]\n");
        printf("Where: \nmode == 0 -> working with many files (use funciton past\n");
        printf("mode == 1 -> working with one file");
        return -1;
    }

    FILE * list = fopen(argv[1],"r");
    processesNumber= atoi(argv[2]);
    maxSeconds = atoi(argv[3]);

    pid_t pidNumbers[processesNumber];
    pid_t statuses[processesNumber];

    countNumberOfPairs(list);
    parseMode(argv[4], list);

    int cpuTime = atoi(argv[5]);
    int virtualMemory = atoi(argv[6]);

    struct rusage childProcessesData[processesNumber];

    for(int i=0; i<processesNumber; i++) {
        pid_t child = fork();
        if (child == -1) {
           printf("Fork error!\n");
           return -1;
        }
        else if(child > 0) pidNumbers[i]=child;
        else {
            struct rlimit *time = (struct rlimit *) calloc(processesNumber, sizeof(struct rlimit));
            struct rlimit *memory = (struct rlimit *) calloc(processesNumber, sizeof(struct rlimit));
            time->rlim_cur = time->rlim_max = cpuTime;
            memory->rlim_cur = memory->rlim_max = virtualMemory * 1e7;
            setrlimit(RLIMIT_AS, memory);
            setrlimit(RLIMIT_CPU, time);
            free(time);
            free(memory);
            exit(processChild(i,atoi(argv[4]), list));
        }
        pidNumbers[i]=wait4(pidNumbers[i],&statuses[i],0, &childProcessesData[i]);
    }

    for(int i=0; i<processesNumber; i++) {
        printf("The process with PID = %d have made %d multiplications\n",(int)pidNumbers[i],WEXITSTATUS(statuses[i]));
        printf("User CPU time: %lf (seconds), System CPU time: %lf (seconds), Maximum resident set size:  %lf (MB)\n\n", childProcessesData[i].ru_utime.tv_sec + childProcessesData[i].ru_utime.tv_usec/(1e6),childProcessesData[i].ru_stime.tv_sec + childProcessesData[i].ru_stime.tv_usec/(1e6), childProcessesData[i].ru_maxrss/(1e3));
    }
    if(mode == 0){
        writeResultToOneFile(list);
    }
    fclose(list);
    printf("\n\n");
    return 0;
}