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
// mode 1 - kill
// mode 2 - sigrt
// mode 3 - sigqueue
int mode = -1;

void sig1Handler(int sig) {
    receivedSignals++;
}

void sig2Handler(int sig, siginfo_t * info, void * ucontext) {
    printf("Program catcher received %d signals.\n", receivedSignals);
    senderPid = info->si_pid;
    isSig2Catched = true;
}

void setMode(char *modeArgument) {
    if(!strcmp(modeArgument, "kill")) {
        mode = 1;
    }
    else if(!strcmp(modeArgument, "sigrt")) {
        mode = 2;
    }
    else if(!strcmp(modeArgument, "sigqueue")) {
        mode = 3;
    }
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        printf("Wrong number of arguments!\n");
        printf("You should add one from these options: kill, sigrt, sigqueue\n");
        return -1;
    }
    pid_t pid = getpid();
    printf("Catcher PID number: %d\n", pid);

    setMode(argv[1]);

    int SIG1, SIG2;
    if(mode == 2){
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

    if(mode == 1|| mode == 2) {
        for(int i = 0; i < receivedSignals; i++) {
            kill(senderPid, SIG1);
        }
        kill(senderPid, SIG2);
    }
    else if(mode == 3) {
        union sigval value;
        value.sival_ptr = NULL;
        value.sival_int = 0;
        for(int i = 0; i < receivedSignals; i++) {
            value.sival_int++;
            sigqueue(senderPid, SIG1, value);
        }
        sigqueue(senderPid, SIG2, value);
    }
    return 0;
}