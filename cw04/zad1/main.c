#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h> //bool

bool flag = false;

void sigintFunction(int signum){
    printf("\nOdebrano sygnal SIGINT\n");
    exit(signum);
}

void sigtstpFunction(int signum){
    if(!flag){
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
    }
    flag=!flag;
}

int main() {

    struct sigaction act;
    act.sa_handler=sigtstpFunction;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGTSTP,&act,NULL);

    signal(SIGINT,sigintFunction);


    while(true){
        if(!flag) {
            system("ls -l");
        }
        sleep(1);
    }
    return 0;
}