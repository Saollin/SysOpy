#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

void error(char *message);
void start();
void parseGenerating(char *argv[], int i, int argc);
void parseSorting(char *argv[], int i, int argc);
void parseCopying(char *argv[], int i, int argc);
void generate(char * fileName, int numOfRecords, int recordSize);
void end();
void sysCopy(char * fileFrom, char * fileTo, int numOfRecords, int recordSize);
void libCopy(char * fileFrom, char * fileTo, int numOfRecords, int recordSize);
void sysSort(char * fileName, int numOfRecords, int recordSize);
int sysQuickSort(int file, int recordSize, int low, int high);
void libSort(char * fileName, int numOfRecords, int recordSize);
int libQuickSort(FILE * file, int recordSize, int low, int high);
int main(int argc, char * argv[]);

#endif