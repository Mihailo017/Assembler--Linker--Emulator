#include "section.h"

#include "list.h"
#include "symtab.h"
#include "allocation.h"
#include "instruction.h"
#include "assembler.h"
#include "rellocation.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_SECTION_SIZE 16384

extern List_Symbol_h symbols;

Section_h section_create(char* name)
{
    Section* s = malloc(sizeof(Section));
    s->section = createSection(name, (Section_h)s);
    list_addTail(symbols, s->section);
    s->id = *s->section;
    list_create(&s->allocList, NULL);
    list_create(&s->insList,  NULL);
    list_create(&s->rellocList, NULL);
    s->size = 0;
    return &s->id;
}

void section_setSize(Section_h handle, uint16_t size)
{
    Section* s = (Section*)handle;
    s->size = size;
}

uint16_t section_getSize(Section_h handle)
{
    return ((Section*)handle)->size;
}

Section_id section_getID(Section_h s)
{
    Section* section = (Section*)s;
    return section->id;
}

void section_setId(Section_h s, uint64_t id)
{
    Section* section = (Section*)s;
    section->id = id;
}

void section_addAllocation(Section_h section, Allocation_h allocation)
{
    Section* s = (Section*)section;
    list_addTail(s->allocList, allocation);
}

void section_addAllocationName(Section_h section, char* name)
{
    Section* s = (Section*)section;
    list_start(s->allocList);
    allocation_addName((Allocation_h)list_get(s->allocList), name);
}

void section_addRellocation(Section_h section, Rellocation_h rellocation)
{
    Section* s = (Section*) section;
    list_addTail(s->rellocList, rellocation);
}

void section_addInstruction(Section_h s, Instruction_h i)
{
    Section* section = (Section*)s;
    list_addTail(section->insList, i);
}

List_h scList = NULL;

typedef struct SectionContent {
    Symbol_h section;
    uint8_t *content;
} SectionContent;

static void freeContent(void* data) {
    SectionContent *sc = (SectionContent*) data;
    free(sc->content);
}

void section_allocateContent(Section_h section)
{
    Section* s = (Section*)section;
    s->content = malloc(s->size);
}

void section_secondPass(Section_h s)
{
    Section* section = (Section*)s;
    if (section->size > 0) {
        section->content = calloc(section->size, sizeof(int8_t));
        for (list_start(section->insList); list_notEnd(section->insList); list_next(section->insList)) {
            Instruction_h instruction = (Instruction_h)list_get(section->insList);
            int8_t* bytes = instruction_getBytes2(instruction);
            section_writeBytes(s, bytes, instruction_getOffset(instruction), instruction_getSize(instruction));
        }
        
        for (list_start(section->allocList); list_notEnd(section->allocList); list_next(section->allocList)) {
            Allocation_h allocation = (Allocation_h)list_get(section->allocList);
             int8_t* bytes = allocation_getBytes(allocation, s);
            section_writeBytes(s, bytes, allocation_getOffset(allocation), allocation_getSize(allocation));  
        }

        if (!list_isEmpty(section->rellocList)) {
            if (symbol_getName(section->section)[0] == '.') {
                fprintf(out, "#.rel%s\ttype\tord\n", symbol_getName(section->section));
            } else {
                fprintf(out, "#.rel.%s\ttype\tord\n", symbol_getName(section->section));
            }
        }
            
        for (list_start(section->rellocList); list_notEnd(section->rellocList); list_next(section->rellocList)) {
            Rellocation_h r = (Rellocation_h)list_get(section->rellocList);
            rellocation_write(r);
        }
    }
        // if (section->size > 0)
        //     fprintf(out, "# %s\n", symbol_getName(section->section));
        // for (uint16_t i = 0; i < section->size; i++) {
        //     if (i % 25 == 0 && i) fprintf(out, "\n");
        //     fprintf(out, "%0*x ", 2, section->content[i]);
        // }
        // fprintf(out, "\n");
}

static SectionContent* findSectionContent(Section_h section)
{
    SectionContent *sc = NULL;
    for (list_start(scList); list_notEnd(scList); list_next(scList)) {
        if ((sc = (SectionContent*)list_get(scList))->section == section) {
            break;
        } 
    }
    return sc;
}

void section_writeWord(Section_h section, int16_t value, uint16_t offset)
{
    Section* s = (Section*)section;
    s->content[offset] = value & 0xFF;
    s->content[offset++] = (value & 0xFF00) >> 8;
}

void section_writeBytes(Section_h section, int8_t* content, uint16_t start, uint16_t cnt)
{
    Section* s = (Section*)section;
    for (uint16_t i=0; i < cnt; i++)
        s->content[start+i] = content[i];
}

void section_delete(void* section)
{
    Section* s = (Section*)section;
    list_delete(s->allocList);
    list_delete(s->insList);
    list_delete(s->rellocList);
    free(s->content);
}

void section_writeContent(Section_h s)
{
    Section* section = (Section*)s;
    if (section->size > 0) {
        fprintf(out, "# %s\n", symbol_getName(section->section));
        for (uint16_t i = 0; i < section->size; i++) {
            if ((i % 25 == 0) && (i > 0)) fprintf(out, "\n");
            fprintf(out, "%0*x ", 2, section->content[i]);
        }
        fprintf(out, "\n");
    } 
}


extern FILE* out;