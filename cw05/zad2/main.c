#include <stdio.h>

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Wrong number of arguments. You should give only name of file\n");
        return -1;
    }
    char * fileName = argv[1];

    char buff[200];
    sprintf(buff, "sort %s", fileName);
    FILE * sortFile = popen(buff, "r");
    
    while(!feof(sortFile)) {
        fgets(buff, sizeof(buff), sortFile);
        printf("%s", buff);
    }

    pclose(sortFile);

    return 0;
}