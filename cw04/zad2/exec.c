#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SIGNAL SIGUSR1


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

    raise(SIGNAL);
    
    if(!strcmp(argv[1], "ignore")){
        printf("Exec process is still running after raise in parent process.\n");
    }
    else if(!strcmp(argv[1], "mask")) {
        printf("Parent process is still running after raise. \n");
    }
    else if(!strcmp(argv[1], "pending")) {
        checkIfSignalIsVisible();
    }
    else {
        printf("Such option doesn't exist!\n");
        printf("You should choose one from these options: ignore, handler, mask, pending. \n");
        return -1;
    }
    return 0;
}