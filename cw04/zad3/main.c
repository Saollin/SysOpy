
#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void sigInfoHandler(int sig, siginfo_t * info, void * ucontext) {
    printf("Signal name: %s\n", strsignal(sig));
    printf("Signal number = %d\n",info->si_signo);
    printf("An errno value: %d\n", info->si_errno);
    printf("Signal code = %d\n",info->si_code);
    printf("Sending process ID (PID) = %d\n",info->si_pid);
    printf("Real user ID of sending process (UID) = %d\n",info->si_uid);
    if(sig == SIGTSTP) {
        printf("Exit value of child process: %d\n", info->si_status);
    }
    else if(sig == SIGCHLD) {
        printf("User time consumed by child process: %ld\n", info->si_utime);
        printf("System time consumed by child process: %ld\n", info->si_stime);
    }
    else if(sig == SIGILL || sig == SIGFPE || sig == SIGSEGV) {
        printf("Memory location which caused fault: %p\n", info->si_addr);
    }
}

void sigChldHandler(int signum){
    exit(0);
}

int main(int argc, char **argv) {
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigInfoHandler;
    int signal = atoi(argv[1]);
    sigaction(signal, &act, NULL);
    if(signal == SIGTSTP) {
        int child = fork();
        if(child == 0) {
            raise(signal);
        }
        int status = 0;
        waitpid(child, &status, 0);
    }
    else if(signal == SIGCHLD) {
        int child = fork();
        if(child == 0) {
            struct sigaction act2;
            sigemptyset(&act2.sa_mask);
            act2.sa_flags = 0;
            act2.sa_handler = sigChldHandler;
            sigaction(SIGCHLD, &act2, NULL);
        }
        if(child > 0) {
            sleep(1);
            kill(child, SIGCHLD);
            int status = 0;
            waitpid(child, &status, 0);
        }
    }
    else
    {
        raise(signal);
    }
    printf("\n\n");
    return 0;
}