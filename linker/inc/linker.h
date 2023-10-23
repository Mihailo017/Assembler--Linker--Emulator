#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdio.h>
#include "section.h"

#define MAX_INPUT_FILES 10
#define MAX_SECTIONS 10

FILE* inputFiles[MAX_INPUT_FILES];
Section* sections[MAX_SECTIONS];
int inputFileIndex;
int sectionIndex;

FILE* out;

Section* placeArgs[MAX_INPUT_FILES * MAX_SECTIONS];
int placeArgsIndex;

#define LINE_LENGTH 100
char* line[LINE_LENGTH];
int symbolFound[MAX_INPUT_FILES], rellocationFound[MAX_INPUT_FILES];

int halted;

int mode;
#define MODE_INITIAL 0
#define MODE_HEX 1
#define MODE_LINKABLE 2

void link();

#endif