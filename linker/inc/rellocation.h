#ifndef _RELLOCATION_H_
#define _RELLOCATION_H_

#include "list.h"

typedef List RellocationList;
RellocationList* rellocationReccords;
#define createRellocationTable() createList()

#define RELLOC_LINE_LENGTH 100

#define RELLOC_RECCORD_LINE "%x\t\t%s\t%d\n"
#define RELLOC_RECCORD_HEADER "#.ret%s\ttype\tord\n"

#define R_PCREL "R_PC16"
#define R_ "R_16"

typedef struct Rellocation {
    int section; // Izvlacis is prve linije npr #ret.text
    char* type; // Ako je pcrel treba se doda i pc
    int symbol; // Redni broj
    int offset;
    int file;
} Rellocation;

Rellocation* createRellocation();
void readRellocationReccords();

void printRelloc();

int isPcRel(Rellocation* r);

#endif