#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SIGNAL SIGUSR1

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
        error("Program has failed to set mask again. \n");
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

int main(int argc, char ** argv) {
    if(argc < 2) {
        printf("Wrong number of arguments!\n");
        printf("You should add one from these options: ignore, handler, mask, pending\n");
        return -1;
    }


    if(!strcmp(argv[1], "ignore")){
        signal(SIGNAL, SIG_IGN);
        raise(SIGNAL);
        printf("Parent process is still running after raise. \n");
    }
    else if(!strcmp(argv[1], "handler")) {
        signal(SIGNAL, sigusrHandler);
        raise(SIGNAL);
    }
    else if(!strcmp(argv[1], "mask")) {
        maskSignal();
        raise(SIGNAL);
        printf("Parent process is still running after raise. \n");
    }
    else if(!strcmp(argv[1], "pending")) {
        maskSignal();
        raise(SIGNAL);
        checkIfSignalIsVisible();
        pid_t child;
    }
    else {
        printf("Such option doesn't exist!\n");
        printf("You should choose one from these options: ignore, handler, mask, pending. \n");
        return -1;
    }
    printf("Now will be called function exec.\n");
    execl("./exec", "main", argv[1], NULL);
    return 0;
}