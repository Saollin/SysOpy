#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h> //struct stat
#include <dirent.h>
#include <ftw.h>
#include <limits.h> //PATH_MAX

void error(char *message) {
    perror(message);
    exit(-1);
}
char *toString(time_t time) {
    char *string = malloc(30 * sizeof(char));
    strftime(string, 30, "%Y-%m-%d,%H:%M:%S", localtime(&time));
    return rstring;
}

