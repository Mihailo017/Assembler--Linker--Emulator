#include "symbol.h"
#include "linker.h"
#include <stdlib.h>
#include <string.h>

Symbol* createSymbol() {
    return malloc(sizeof(Symbol));
}

void readSymbolTable() {
    for (int i=0; i < inputFileIndex; i++) {

        if (!symbolFound[i]) continue;

        size_t length = LINE_LENGTH;

        /* line[i] je vec procitan od strane readSectionTable
        * Citaj sve do naredne linije koja pocinje sa #, to su relokacioni zapisi
        */
        while (line[i][0] != '#') {

            char* name = malloc(sizeof(char) * SYMBOL_NAME_LENGTH);
            Symbol* symbol = createSymbol();
            int temp;
            sscanf(line[i], SYMTAB_LINE, &symbol->ordinal, &symbol->section, 
            &symbol->value, &symbol->linking,
            &temp, name);
            if (symbol->linking != 'E') symbol->file = i;
            else symbol->file = -1;

            Symbol* temp1 = findByName(&symbolTable, name);
            if (temp1) {
                if (temp1->linking != 'G') {
                    printf("Pronadjeno vise simbola sa istim imenom!\n Kraj programa\n");
                    exit(1);
                } else { // temp1 je Globalan
                    if (symbol->linking == 'L' || symbol->linking == 'Q') {
                        printf("Pronadjeno vise simbola sa istim imenom!\n Kraj programa\n");
                        exit(1);
                    } else { // temp1 je Globalan, symbol je Globalan ili Eksterni
                        if (temp1->value && symbol->linking != 'E') {
                            printf("Pronadjeno vise simbola sa istim imenom!\n Kraj programa\n");
                            exit(1);
                        } else {
                            if (!temp1->value) {
                                temp1->value = symbol->value;
                                temp1->file = symbol->file;
                                temp1->section = symbol->section; 
                                temp1->ordinalInFile[i] = symbol->ordinal;  
                            }
                        }
                    }
                }
            } else {
                if (symbol->linking == 'E') symbol->linking = 'G';
                char* newName = malloc(strlen(name));
                strcpy(newName, name);
                symbol->name = newName;
                symbol->ordinalInFile[i] = symbol->ordinal;
                addTail(&symbolTable, symbol);
            }

            getline(&line[i], &length, inputFiles[i]);

        }
        rellocationFound[i] = 1;
    }   
}

Symbol* findByName(SymbolTable** symbolTable, char* name) {
    for (currentToHead(symbolTable); hasCurrent(symbolTable); next(symbolTable)) {
        Symbol* symbol = (Symbol*)(getCurrent(symbolTable));
        if (strcmp(symbol->name, name) == 0) return symbol;
    }
    return NULL;
}

Symbol* findByOrdinal(SymbolTable** symbolTable, int ord) {
    for (currentToHead(symbolTable); hasCurrent(symbolTable); next(symbolTable)) {
        Symbol* symbol = (Symbol*)(getCurrent(symbolTable));
        if (symbol->ordinal == ord) return symbol;
    }
    return NULL;
}

Symbol* findByOrdinalInFile(SymbolTable** symbolTable, int ordInFile, int file)
{
    for (currentToHead(symbolTable); hasCurrent(symbolTable); next(symbolTable)) {
        Symbol* symbol = (Symbol*)(getCurrent(symbolTable));
        if (symbol->ordinalInFile[file] == ordInFile) return symbol;
    }
    return NULL;
}

void printSymbolTable() {
    fprintf(out, "ord\tfile\tvalue\tlin\tsec\tname\n");
    for (currentToHead(&symbolTable); hasCurrent(&symbolTable); next(&symbolTable)) {
        Symbol* s = (Symbol*)getCurrent(&symbolTable);
        fprintf(out, "%d\t%d\t%d\t%c\t%d\t%s\n", s->ordinal, s->file, s->value, s->linking, s->section, s->name);
    }
    fprintf(out, "\n");
}