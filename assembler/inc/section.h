#ifndef _SECTION_H_
#define _SECTION_H_

#include "instruction.h"
#include "allocation.h"

List_Sections_h sections;

typedef struct Section {
    Section_id id;
    Symbol_h section;
    List_Instruction_h insList;
    List_Allocation_h allocList;
    List_Rellocation_h rellocList;
    uint8_t *content;
    uint16_t size;
} Section;

Section_h section_create(char* name);

void section_setSize(Section_h section, uint16_t size);
uint16_t section_getSize(Section_h section);

Section_id section_getID(Section_h);
void section_setId(Section_h, uint64_t);

void section_addAllocation(Section_h, Allocation_h);
void section_addAllocationName(Section_h section, char* name);

void section_addRellocation(Section_h section, Rellocation_h rellocation);

void section_addInstruction(Section_h section, Instruction_h instruction);

void section_allocateContent(Section_h section);

void section_secondPass(Section_h section);

void section_writeWord(Section_h section, int16_t value, uint16_t offset);

void section_writeBytes(Section_h section, int8_t* content, uint16_t start, uint16_t cnt);

void section_delete(void*);

void section_writeContent(Section_h);

void writeToSection2(
    int16_t value,
    uint8_t length,
    uint16_t offset,
    Symbol_h section
);

void writeSectionsToFile2(void);

#endif