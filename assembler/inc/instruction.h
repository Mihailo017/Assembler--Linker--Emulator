#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <stdint.h>
#include "list.h"
#include "types.h"


typedef struct Instruction {
    Instruction_id id;
    uint8_t instructionDescriptor;
    uint8_t opCode;
    uint8_t regsDescr;
    uint8_t adrMode;
    int16_t operand;
    uint16_t offset;
    char* operandName;
    uint16_t size; 
    Section_h section;   
} Instruction;

Instruction_h instruction_create(char* name, uint8_t descriptor, uint16_t offset, Section_h section);

void instruction_setInstructionDescriptor(Instruction_h, uint8_t);
uint8_t instruction_getInstructionDescriptor(Instruction_h);

void instruction_setRegsDescr(Instruction_h, uint8_t, uint8_t);
void instruction_setRegDst(Instruction_h, uint8_t);
void instruction_setRegSrc(Instruction_h, uint8_t);

void instruction_setAddressingMode(Instruction_h, uint8_t);

void instruction_setOperandName(Instruction_h, char*);
char* instruction_getOperandName(Instruction_h);

void instruction_setOperand(Instruction_h, int16_t);

int8_t* instruction_getBytes(Instruction_h);
int8_t* instruction_getBytes2(Instruction_h);

uint16_t instruction_getOffset(Instruction_h);
uint16_t instruction_getSize(Instruction_h);

void instruction_setOffset(Instruction_h, uint16_t);

Section_id instruction_getOperandSectionID(Instruction_h i);

#define createInstructionList(handle) createList2(&handle ,  NULL)
#define freeInstructionList(handle) emptyList2(handle);

#endif