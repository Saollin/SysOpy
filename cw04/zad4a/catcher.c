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

void sig1Handler(int sig) {
    receivedSignals++;
}

void sig2Handler(int sig, siginfo_t * info, void * ucontext) {
    printf("Program catcher received %d signals.\n", receivedSignals);
    senderPid = info->si_pid;
    isSig2Catched = true;
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        printf("Wrong number of arguments!\n");
        printf("You should add one from these options: kill, sigrt, sigqueue\n");
        return -1;
    }
    pid_t pid = getpid();
    printf("Catcher PID number: %d\n", pid);

    int SIG1, SIG2;
    if(!strcmp(argv[1], "sigrt")){
        SIG1 = SIGRTMIN + 2;
        SIG2 = SIGRTMIN + 3;
    }
    else {
        SIG1 = SIGUSR1;
        SIG2 = SIGUSR2;
    }   

    struct sigaction act1;
    sigemptyset(&act1.sa_mask);
    // sigdelset(&act1.sa_mask, SIG1);
    // sigdelset(&act1.sa_mask, SIG2);
    act1.sa_flags = 0;
    act1.sa_handler = sig1Handler;

    // signal(SIG1, sig1Handler);
    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = sig2Handler;
    
    sigaction(SIG1, &act1, NULL);
    sigaction(SIG2, &act2, NULL);

    while(!isSig2Catched);

    if(!strcmp(argv[1], "kill") || !strcmp(argv[1], "sigrt")) {
        for(int i = 0; i < receivedSignals; i++) {
            kill(senderPid, SIG1);
        }
        kill(senderPid, SIG2);
    }
    else if(!strcmp(argv[1], "sigqueue")) {
        union sigval value;
        value.sival_ptr = NULL;
        value.sival_int = 0;
        for(int i = 0; i < receivedSignals; i++) {
            sigqueue(senderPid, SIG1, value);
            value.sival_int++;
        }
        sigqueue(senderPid, SIG2, value);
    }
    return 0;
}