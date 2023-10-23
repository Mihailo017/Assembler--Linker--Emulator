#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdio.h>


int textSectionStart;

extern FILE* out;


#define LINE_LENGTH 100
char* line[LINE_LENGTH];

int halted;

void emulate(void);

#endif