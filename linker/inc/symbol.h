#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "list.h"

typedef List SymbolTable;
SymbolTable* symbolTable;

#define SYMTAB_LINE "%d\t%d\t%x\t%c\t%x\t%s\n"
#define SYMTAB_HEADER "# symbol table\n"
#define SYMBOL_NAME_LENGTH 50

#define createSymbolTable() createList()

typedef struct Symbol {
    int ordinal;
    int value;
    char linking; // Ako je globalan smesta se na posebno mesto
    int section;
    int file;
    char* name;
    int ordinalInFile[10];
} Symbol;

Symbol* createSymbol();

void readSymbolTable();

Symbol* findByName(SymbolTable**, char*);
Symbol* findByOrdinal(SymbolTable**, int);

Symbol* findByOrdinalInFile(SymbolTable**, int, int);

void printSymbolTable();


#endif