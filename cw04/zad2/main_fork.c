#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void error(char *message) {
    perror(message);
    exit(-1);
}

void sigusrHandler(int signum){
    printf("\nOdebrano sygnal SIGUSR1\n");
}

void maskSignal() {
    sigset_t newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, SIGUSR1);
    if(sigprocmask(SIG_SETMASK, &newMask, NULL)) {
        error("Program has failed to set mask again");
    }
}

void raiseSignalInChildProcess() {
        pid_t child;
        child = fork();
        if(child == 0) {
            raise(SIGNAL);
            printf("Child process is still running after raise in parent process.\n");
    }
    }

void checkIfSignalIsVisible() {
    sigset_t set;
            sigpending(&set);
            if(sigismember(&set, SIGNAL)) {
                printf("SIGUSR1 is visible.\n");
    }
            else {
                printf("SIGUSR1 is not visible.\n");
    }
    }
    else {
        printf("Such option doesn't exist!");
        printf("You should choose one from these options: ignore, handler, mask, pedning");
        return -1;
    }

    raise(SIGUSR1);
    pid_t child;

    if(!strcmp(argv[1], "ignore")){
        child = fork();
        if(child == 0) {
            raise(SIGUSR1);
        }
    }
    else if(!strcmp(argv[1], "handler")) {
        child = fork();
        if(child == 0) {
            raise(SIGUSR1);
        }
    }
    else if(!strcmp(argv[1], "mask")) {
        child = fork();
        if(child == 0) {
            raise(SIGUSR1);
        }
    }
    else if(!strcmp(argv[1], "pending")) {
        child = fork();
        if(child == 0) {
            sigset_t set;
            sigpending(&set);
            if(sigismember(&set, SIGUSR1)) {
                printf("SIGUSR1 is visible.\n");
            }
            else {
                printf("SIGUSR1 is not visible.\n");
            }
        }
    }
    
}