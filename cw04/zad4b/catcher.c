#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

int SIG1, SIG2;
bool isSig2Catched = false;
int receivedSignals = 0;
pid_t senderPid = -1;
// mode 1 - kill
// mode 2 - sigrt
// mode 3 - sigqueue
int mode = -1;

void sendSignal(int signal) {
    if(mode == 1|| mode == 2) {
        kill(senderPid, signal);
    }
    else if(mode == 3) {
        union sigval value;
        value.sival_ptr = NULL;
        value.sival_int = receivedSignals;
        sigqueue(senderPid, signal, value);
    }
}

void sig1Handler(int sig, siginfo_t * info, void * ucontext_t) {
    if(senderPid == -1) {
        senderPid = info->si_pid;
    }
    receivedSignals++;
    sendSignal(SIG1);
}

void sig2Handler(int sig, siginfo_t * info, void * ucontext) {
    printf("Program catcher received %d signals.\n", receivedSignals);
    isSig2Catched = true;
    sendSignal(SIG2);
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

    if(mode == 2){
        SIG1 = SIGRTMIN + 2;
        SIG2 = SIGRTMIN + 3;
    }
    else {
        SIG1 = SIGUSR1;
        SIG2 = SIGUSR2;
    }   
    //block all signals
    sigset_t newSet, oldSet, mask;
    sigfillset(&newSet);
    if(sigprocmask(SIG_BLOCK, &newSet, &oldSet)) {
        perror("Program failed to block signals");
    }

    sigemptyset(&mask);
    sigaddset(&mask, SIG1);
    sigaddset(&mask, SIG2);
    if(sigprocmask(SIG_UNBLOCK, &mask, NULL)) {
        perror("Program failed to unblock signals");
    }

    struct sigaction act1;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = SA_SIGINFO;
    act1.sa_sigaction = sig1Handler;

    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = sig2Handler;
    
    sigaction(SIG1, &act1, NULL);
    sigaction(SIG2, &act2, NULL);

    while(!isSig2Catched) {
        pause();
    };

    return 0;
}