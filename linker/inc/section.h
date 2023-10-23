#ifndef _SECTION_H_
#define _SECTION_H_

#include "rellocation.h"
#include <stdio.h>

#define SECTION_NAME_LENGTH 100

typedef struct  Section {
    int ordinal;
    int start;
    int size;
    int accuSize;
    int file;
    int placed;
    char* name;
    RellocationList* rellocations;
    int positionInFile;
    int ordinalInFile[10];
} Section;

Section* createSection(char* name, int start);

void readSectionTable();

void printSectionTable();

int sumSize();
int maxEnd();

#endif