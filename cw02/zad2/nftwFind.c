#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h> //struct stat
#include <dirent.h>
#include <ftw.h>
#include <limits.h> //PATH_MAX

int depth = INT_MAX - 1;
int globalSgn = -1;

void error(char *message) {
    perror(message);
    exit(-1);
}

char *toString(time_t time) {
    char *string = malloc(40 * sizeof(char));
    if(strftime(string, 40, "%Y-%m-%d,%H:%M:%S", localtime(&time)) == 0) {

    }
    return rstring;
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


    printf("Path of file: %s\n
    Type: %s\n,
    Links count %hu\n: 
    Size: %d\n
    Access time: %s\n
    Modification time: %s\n\n",
    full_path, type, stats->st_nlink, (int) stats->st_size,toString(stats->st_atime), toString(stats->st_mtime));
}

void nftwSearch(char *dirName) {
    nftw(*dirName, nftwFunc, 10, FTW_PHYS);
}

int nftwFunc(const char *path, const struct *stats, in fd, struct FTW *flag)
{
    if (ftwbuf->level > globalDepth + 1|| strcmp(fullPath, ".") == 0){
        return 0;
    }
    struct tm *modTime = localtime(&stat->st_mtime);
    struct tm *accTime = localtime(&stat->st_atime);

    char *fullPath = (char *) calloc(PATH_MAX, sizeof(char));
    realpath(path, fullPath);     
    
    switch (modeGlobal)
        {
        case 0:
            nftwPrint(fullPath, stat);
            break;
        
        case 1:
            if (checkTime(accTime, sgnGlobal, nGlobal) > 0){
                nftwPrint(fullPath, stat);
            }
            break;
        
        case 2:
            if (checkTime(modTime, sgnGlobal, nGlobal) > 0){
                nftwPrint(fullPath, stat);
            }

        default:
            break;
        }
    return 0;
}
