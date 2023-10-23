#include "instruction.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "section.h"
#include "instruction.h"
#include "rellocation.h"

#define NUM_OF_INSTRUCTIONS 24
#define MAX_INS_NAME_LENGTH 5

static char instruction_names[NUM_OF_INSTRUCTIONS][MAX_INS_NAME_LENGTH] = {
"halt", "int", "iret", "call", "ret",
"jmp", "jeq", "jne", "jgt", "xchg",
"add", "sub", "mul", "div", "cmp",
"not", "and", "or", "xor", "test",
"shl", "shr", "ldr", "str"};

#define HALT 0x00
#define INT 0x10
#define IRET 0x20
#define CALL 0x30
#define RET 0x40
#define JMP 0x50
#define JEQ 0x51
#define JNE 0x52
#define JGT 0x53
#define XCHG 0x60
#define ADD 0x70
#define SUB 0x71
#define MUL 0x72
#define DIV 0x73
#define CMP 0x74
#define NOT 0x80
#define AND 0x81
#define OR 0x82
#define XOR 0x83
#define TEST 0x84
#define SHL 0x90
#define SHR 0x91
#define LDR 0xA0
#define STR 0xB0


static uint8_t instruction_opCodes[NUM_OF_INSTRUCTIONS] = {
    HALT, INT, IRET, CALL, RET,
    JMP, JEQ, JNE, JGT, XCHG,
    ADD, SUB, MUL, DIV, CMP,
    NOT, AND, OR, XOR, TEST,
    SHL, SHR, LDR, STR
};

static uint64_t nextId = 0;

Instruction_h instruction_create(char* name, uint8_t descriptor, uint16_t offset, Section_h section)
{
    Instruction* instruction = calloc(1, sizeof(Instruction));
    instruction->id = ++nextId;
    instruction->instructionDescriptor = descriptor;
    instruction->section = section;
    instruction->offset = offset;
    instruction->size = 0;
    instruction->adrMode = 0xFF;

    for (int i=0; i< NUM_OF_INSTRUCTIONS; i++)
        if (strcmp(name, instruction_names[i]) == 0) {
            instruction->opCode = instruction_opCodes[i];
            break;
        }
    if (descriptor == PUSH) instruction->opCode = STR;
    if (descriptor == POP) instruction->opCode = LDR;
     
    instruction->size++;
    return (Instruction_h)instruction;
}

void instruction_setInstructionDescriptor(Instruction_h i, uint8_t desc)
{
    Instruction* instruction = (Instruction*)i;
    instruction->instructionDescriptor = desc;
}
uint8_t instruction_getInstructionDescriptor(Instruction_h i)
{
    Instruction* instruction = (Instruction*)i;
    return instruction->instructionDescriptor;
}

void instruction_setRegsDescr(Instruction_h i, uint8_t dst, uint8_t src)
{
    Instruction* instruction = (Instruction*)i;
    instruction->regsDescr = (dst << 4) | src;
    instruction->size = instruction->size < 2 ? 2 : instruction->size;
}

void instruction_setRegDst(Instruction_h i, uint8_t dst)
{
    Instruction* instruction = (Instruction*)i;
    instruction->regsDescr = (dst << 4) | (instruction->regsDescr & 0x0F);
    instruction->size = instruction->size < 2 ? 2 : instruction->size;
}

void instruction_setRegSrc(Instruction_h i, uint8_t src)
{
    Instruction* instruction = (Instruction*)i;
    instruction->regsDescr = (instruction->regsDescr & 0xF0) | src;
    instruction->size = instruction->size < 2 ? 2 : instruction->size;
}

void instruction_setAddressingMode(Instruction_h i, uint8_t addrMode)
{
    Instruction* instruction = (Instruction*)i;
    instruction->adrMode = 0x00 | addrMode;
    instruction->size = instruction->size < 3 ? 3 : instruction->size;
}

void instruction_setOperandName(Instruction_h i, char* name)
{
    Instruction* instruction = (Instruction*)i;
    if (instruction->operandName == NULL)
        instruction->operandName = malloc(strlen(name));
    strcpy(instruction->operandName, name);
    instruction->size = 5;
}

char* instruction_getOperandName(Instruction_h i)
{
    Instruction* instruction = (Instruction*)i;
    return instruction->operandName;
}

void instruction_setOperand(Instruction_h i, int16_t operand)
{
    Instruction* instruction = (Instruction*)i;
    instruction->operand = operand;
}

extern List_Symbol_h symbols;
int8_t* instruction_getBytes(Instruction_h i)
{
    Instruction* instruction = (Instruction*)i;
    int8_t* bytes = malloc(instruction->size);
    uint16_t j = 0;
    bytes[j++] = instruction->opCode;
    bytes[j++] = instruction->regsDescr;
    if (instruction->adrMode != 0xFF) {
        bytes[j++] = instruction->adrMode;
    }
    if (instruction->operandName != NULL) {
        int16_t value = 0;
        if (isLiteral(instruction->operandName)) {
            value = processLiteral2(instruction->operandName);
        } else {
            Symbol_h symbol = list_symbol_find(symbols, instruction->operandName);
            value = symbol_isGlobal(symbol) ? 0 : symbol_getOffset(symbol);
        }
        bytes[j++] = value & 0x00FF;
        bytes[j++] = (value & 0xFF00) >> 8;
    }
    return bytes;
}

int8_t* instruction_getBytes2(Instruction_h i)
{
    Instruction* instruction = (Instruction*)i;
    int8_t* bytes = malloc(instruction->size);
    uint16_t j = 0;
    bytes[j++] = instruction->opCode;
    bytes[j++] = instruction->regsDescr;
    if (instruction->adrMode != 0xFF) {
        bytes[j++] = instruction->adrMode;
    }
    if (instruction->operandName != NULL) {
        int16_t value = 0;
        if (isLiteral(instruction->operandName)) {
            value = processLiteral2(instruction->operandName);
        } else {
            Symbol_h operand = list_symbol_find(symbols, instruction->operandName);
            value = symbol_isGlobal(operand) ? 0 : symbol_getOffset(operand);
            uint8_t addressing = !PCREL;
            if ((bytes[2] & 0x0F) == REGDIRREL) {
                if (instruction->section == symbol_getSection(operand)) {
                    uint16_t nextInstructionStart = instruction->offset + instruction->size;
                    value -= nextInstructionStart;
                } else 
                    value -= 2;
                addressing = PCREL;
            }
            if (symbol_getLinking(operand) != 'A') {
                Rellocation_h r = rellocation_create(operand, instruction->offset+3, addressing);
                section_addRellocation(instruction->section, r);
            }
        }
        
        bytes[j++] = value & 0x00FF;
        bytes[j++] = (value & 0xFF00) >> 8;
    }
    return bytes;
}

uint16_t instruction_getOffset(Instruction_h i)
{
    Instruction* instruction = (Instruction*)i;
    return instruction->offset;
}

uint16_t instruction_getSize(Instruction_h i)
{
    Instruction* instruction = (Instruction*)i;
    return instruction->size;
}

void instruction_setOffset(Instruction_h i, uint16_t offset)
{
    Instruction* instruction = (Instruction*)i;
    instruction->offset = offset;
}