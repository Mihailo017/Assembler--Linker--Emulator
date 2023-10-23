#include "symtab.h"

#include "assembler.h"
#include "section.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>


extern Symbol_h currSec;


static uint64_t newSymbolOrdinal = 0;


Symbol_h symbol_create(
    char* name,
    uint16_t offset,
    bool defined,
    char linking,
    bool isSection,
    Section_h section
    )
{
    Symbol* s = calloc(1, sizeof(Symbol));
    s->id = ++newSymbolOrdinal;

    if (isSection) {
        s->section = section;
    } else if (defined) {
        s->section = currSec;
    }

    if (linking == 'E' || linking == 'A') {
        s->section = calloc(1, sizeof(Section_id));
    }

    s->offset = offset;
    s->defined = defined;
    s->linking = linking;
    s->isSection = isSection;

    strcpy(s->name, name);

    return &s->id;
}

extern FILE* out;
void list_symbol_write(List_Symbol_h symbols)
{
    fprintf(out, "# symbol table\n");
    fprintf(out, "#ord\tsec\toff\tlin\tsize\tname\n");

    uint64_t j = 0;
    for (list_start(symbols); list_notEnd(symbols); list_next(symbols)) {
        Symbol* s = (Symbol*)list_get(symbols);
        if (s->isSection) {
            *s->section = j+1;
            s->id = ++j;
            fprintf(out, "%" PRId64 "\t%" PRIu64 "\t", s->id, *s->section);
            fprintf(out, "%0*x\t", 4, s->offset & 0xFFFF);
            fprintf(out, "%c\t", s->linking); 
            fprintf(out, "%0*x\t", 4, section_getSize(s->section));
            fprintf(out, "%s\n", s->name);
            
        }
    }

    for (list_start(symbols); list_notEnd(symbols); list_next(symbols)) {
        Symbol* s = (Symbol*)list_get(symbols);
        if (!s->isSection) {
            s->id = ++j;
            fprintf(out, "%" PRId64 "\t%" PRId64 "\t",s->id, *s->section);
            fprintf(out, "%0*x\t", 4, s->offset);
            fprintf(out, "%c\t%d\t%s\n", s->linking, 2, s->name);
            
        }
    }
}

Symbol_h list_symbol_find(List_Symbol_h handle, char* name) {
    if (handle == NULL) return NULL;
    for (list_start(handle); list_notEnd(handle); list_next(handle)) {
        Symbol* s = (Symbol*)(list_get(handle));
        if (strcmp(s->name, name) == 0) return &s->id;
    }
    return NULL;
}

char symbol_getLinking(Symbol_h symbol)
{
    Symbol *s = (Symbol*) symbol;
    return s->linking;
}

void symbol_setLinking(Symbol_h handle, char linking)
{
    Symbol *s = (Symbol*) handle;
    s->linking = linking;
}

bool symbol_isGlobal(List_Symbol_h handle)
{
    return ((Symbol*) handle)->linking == 'G' || ((Symbol*) handle)->linking == 'E';
}

Symbol_id symbol_getID(Symbol_h symbol)
{
    return *symbol;
}

void symbol_setSection(Symbol_h symbol, Section_h section)
{
    Symbol *s = (Symbol*) symbol;
    s->section = section;
}

Section_h symbol_getSection(Symbol_h handle)
{
    return ((Symbol*)handle)->section;
}

uint16_t symbol_getOffset(List_Symbol_h handle)
{
    return ((Symbol*) handle)->offset;
}

char* symbol_getName(Symbol_h handle)
{
    Symbol *s = (Symbol*) handle;

    return s->name;
}