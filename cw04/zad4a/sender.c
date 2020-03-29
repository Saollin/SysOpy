#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

bool isSig2Catched = false;
int receivedSignals = 0;
pid_t senderPid;

union sigval value = {.sival_ptr = NULL};

void sig1Handler(int sig) {
    receivedSignals++;
}

void sig2Handler(int sig, siginfo_t * info, void * ucontext) {
    printf("Number or receiving signal: %d\n", info->si_value.sival_int);
    isSig2Catched = true;
}

int main(int argc, char ** argv) {
    if(argc < 4) {
        printf("Wrong number of arguments!\n");
        printf("Run program like this: ./catcher [catcher PID] [number of signals] [kill|sigrt|sigqueue]");
        return -1;
    }

    int SIG1, SIG2;
    if(!strcmp(argv[3], "sigrt")){
        SIG1 = SIGRTMIN + 2;
        SIG2 = SIGRTMIN + 3;
    }
    else {
        SIG1 = SIGUSR1;
        SIG2 = SIGUSR2;
    }   
    pid_t catcherPid = atoi(argv[1]);
    int numberOfSignals = atoi(argv[2]);

    struct sigaction act1;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    act1.sa_handler = sig1Handler;

    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = sig2Handler;
    
    sigaction(SIG1, &act1, NULL);
    sigaction(SIG2, &act2, NULL);

    if(!strcmp(argv[3], "kill") || !strcmp(argv[3], "sigrt")) {
        for(int i = 0; i < numberOfSignals; i++) {
            kill(catcherPid, SIG1);
        }
        kill(catcherPid, SIG2);
    }
    else if(!strcmp(argv[3], "sigqueue")) {
        for(int i = 0; i < numberOfSignals; i++) {
            sigqueue(catcherPid, SIG1, value);
        }
        sigqueue(catcherPid, SIG2, value);
    }
    while(!isSig2Catched);

    printf("Program sender received %d signals.\n", receivedSignals);
    printf("It should receive %d signals. \n", numberOfSignals);
    return 0;
}