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
