#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "list.h"
#include "types.h"
#include <stdbool.h>


#define SYMBOL_NAME_LENGTH 64

typedef struct Symbol {
    Symbol_id id;
    Section_h section;
    uint16_t offset;
    bool defined;
    char linking;
    bool isSection;
    char name[SYMBOL_NAME_LENGTH];
} Symbol;

#define createSection(name, section) \
    symbol_create(name, 0, true, 'L', true, section)

#define createDefinedSymbol(name, linking) \
    symbol_create(name, locationCounter, true, linking, false, NULL)

#define createExternSymbol(name) \
    symbol_create(name, 0, false, 'E', false, NULL)

#define createAbsoluteSymbol(name, offset) \
    symbol_create(name, offset, false, 'A', false, NULL)    

Symbol_h symbol_create(
    char* name,
    uint16_t offset,
    bool defined,
    char linking,
    bool isSection,
    Section_h section
);

void list_symbol_write(List_Symbol_h);
Symbol_h list_symbol_find(List_Symbol_h handle, char* name);

char symbol_getLinking(Symbol_h symbol);
void symbol_setLinking(Symbol_h handle, char linking);

bool symbol_isGlobal(List_Symbol_h handle);

Symbol_id symbol_getID(Symbol_h);

void symbol_setSection(Symbol_h symbol, Section_h section);
Section_h symbol_getSection(Symbol_h handle);

uint16_t symbol_getOffset(List_Symbol_h handle);

char* symbol_getName(Symbol_h handle);

#endif
