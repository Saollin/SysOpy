#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h> //struct stat
#include <dirent.h>
#include <ftw.h>
#include <limits.h> //PATH_MAX

int globalDepth = INT_MAX - 1;

// -1 is -, 1 is + in mtime/atime option, 0 if not specified
int globalSgn = 0;

// 0 - atime or mtime wasn't used
// 1 - atime used
// 2 - mtime used
int globalMode = 0;

//argument n in mtime or atime
int globalN = -1;

time_t currentTime;

void error(char *message) {
    perror(message);
    exit(-1);
}

char *toString(time_t time) {
    char *string = malloc(40 * sizeof(char));
    if(strftime(string, 40, "%Y-%m-%d,%H:%M:%S", localtime(&time)) == 0) {

    }
    return string;
}

int checkTime(struct tm *checkedTime, int sgn, int n){
    int checked = 0;
    checked = checkedTime->tm_mday + 31*checkedTime->tm_mon + 365*checkedTime->tm_year;
   

    struct tm *currTime;
    currTime = localtime(&currentTime);

    //dates represented as sum of days
    int curr = 0;
    curr = currTime->tm_mday + 31*currTime->tm_mon + 365*currTime->tm_year;
    
    //printf("%d  %d\n", curr, checked);
    
    if (sgn == 0){
        if (curr - checked == globalN) return 1;
        else return 0;
    }
    else if(sgn == 1)
    {
        if(curr - checked > globalN) return 1;
        else return 0;
    }
     else if(sgn == -1)
    {
        if(curr - checked < globalN) return 1;
        else return 0;
    }
    return 0;
}

void nftwPrint(const char *fullPath, const struct stat * stats){

    char *type="unknown";
    if(S_ISREG(stats->st_mode))type="file";
    else if(S_ISDIR(stats->st_mode))type="dir";
    else if(S_ISCHR(stats->st_mode))type="char dev";
    else if(S_ISBLK(stats->st_mode))type="block dev";
    else if(S_ISFIFO(stats->st_mode))type="fifo";
    else if(S_ISLNK(stats->st_mode))type="slink";
    else if(S_ISSOCK(stats->st_mode))type="sock";


    printf("Path of file: %s\nType: %s\nLinks count: %lu\nSize: %d\nAccess time: %s\nModification time: %s\n\n",
    fullPath, type, stats->st_nlink, (int) stats->st_size,toString(stats->st_atime), toString(stats->st_mtime));
}


int nftwFunc(const char *path, const struct stat *stats, int fd, struct FTW *flag)
{
    if (flag->level > globalDepth + 1|| strcmp(path, ".") == 0){
        return 0;
    }
    struct tm *modTime = localtime(&stats->st_mtime);
    struct tm *accTime = localtime(&stats->st_atime);

    char *fullPath = (char *) calloc(PATH_MAX, sizeof(char));
    realpath(path, fullPath);     
    
    switch (globalMode)
        {
        case 0:
            nftwPrint(fullPath, stats);
            break;
        
        case 1:
            if (checkTime(accTime, globalSgn, globalN) > 0){
                nftwPrint(fullPath, stats);
            }
            break;
        
        case 2:
            if (checkTime(modTime, globalSgn, globalN) > 0){
                nftwPrint(fullPath, stats);
            }

        default:
            break;
        }
    return 0;
}

void nftwSearch(char *dirName) {
    nftw(dirName, nftwFunc, 10, FTW_PHYS);
}

void parseTimeArgument(char *argument){
    globalN = abs(atoi(argument));
    printf("%d", globalN);
    if(argument[0] == '+') {
        globalSgn = 1;      
    }
    else if(argument[0] == '-') {
        globalSgn = -1;
    }
    else {
        globalSgn = 0;
    }
}

void printHelp(char * message) {
    printf("Usage of program is: \n ./main path [-maxdepth depth] [-atime n or -mtime n]\n");
    if(message != NULL) {
        error(message);
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printHelp(NULL);
        return 1;
    }

    time(&currentTime);
    // ./main path
    if(argc == 2) {
        nftwSearch(argv[1]);
    }
    else if (argc == 4) {
        // ./main path -maxdepth n
        if(!strcmp(argv[2], "-maxdepth")) {
            globalDepth = atoi(argv[3]);
            nftwSearch(argv[1]);
        }
        // ./main path -atime [+-]n
        else if(!strcmp(argv[2], "-atime")) {
            globalMode = 1; //atime mode
            parseTimeArgument(argv[3]);
            nftwSearch(argv[1]);
        }
        // ./main path -mtime [+-]n
        else if(!strcmp(argv[2], "-mtime")) {
            globalMode = 2; //mtime mode
            parseTimeArgument(argv[3]);
            nftwSearch(argv[1]);
        }
        else {
            printHelp("Wrong options!\n");
        }
    }
    else if (argc == 6) {
        // maxdepth should be before rest of options
        if(!strcmp(argv[2], "-maxdepth")) {
            globalDepth = atoi(argv[3]);
            
            if(!strcmp(argv[4], "-atime")){
                globalMode = 1;
                parseTimeArgument(argv[5]);
                nftwSearch(argv[1]);
            }
            else if(!strcmp(argv[4], "-mtime")){
                globalMode = 2;
                parseTimeArgument(argv[5]);
                nftwSearch(argv[1]);
            }
            else {
                printHelp("Wrong options!\n");
            }
        }
        else {
            printHelp("Wrong options!\n");
        }
    }
    else {
        printHelp("Wrong options!\n");
    }
    return 0;
}
