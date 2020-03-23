#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int printLs(char *dirName){

    pid_t child;
    child = fork();

    if(child == 0){
        printf("Path: %s\nPID procesu: %d\n",dirName,(int)getpid());
        execlp("/bin/ls", "ls", "-l", dirName, NULL);
    }
    else {
        int status;
        wait(&status);
        if(status!=0) {
            return status;
        }
    }

    DIR *dir;
    if((dir=opendir(dirName))==NULL){
        printf("Dir not found");
        exit(1);
    }
    struct dirent *curr;
    struct stat currentStat;
    char newPath[4096];
    while ((curr = readdir(dir)) != NULL) {
        if (strcmp(curr->d_name, ".") == 0 || strcmp(curr->d_name, "..") == 0) {
            continue;
        }
        strcpy(newPath,dirName);

        if(dirName[strlen(dirName)-1]!='/')strcat(newPath,"/");

        strcat(newPath,curr->d_name);

        if (lstat(newPath, &currentStat) < 0) {
            printf("Problem with function lstat");
            exit(1);
        }

        if (S_ISDIR(currentStat.st_mode)) {
            printLs(newPath);
        }

    }
    closedir(dir);
    return 0;
}

int main(int argc, char **argv) {

    if(argc != 2){
        printf("Not enough arguments");
        exit(1);
    }

    printLs(argv[1]);

    return 0;
}