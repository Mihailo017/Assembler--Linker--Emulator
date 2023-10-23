#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "instruction.h"

#include <stdio.h>
#include <stdbool.h>


#define WORDSIZE 2

/*
*   Vrednosti koje pozivom yylex fje smestaju u promenljivu currentlyProcessing.
*/
enum CurrentlyProcessing {
    SECTION = 0, LABEL, ALLOCC_WORD,
    SKIP, EQU, GLOBAL, EXTERN, SYMBOL,
    LISTED, EXPRESSION, EOF_REACHED, INITIAL_VALUE, INSTRUCTION, INSTRUCTION_JUMP, COMMENT
};
enum CurrentlyProcessing currentlyProcessing;

// Vrednosti koje yylex vraca kad match-uje instrukciju
#define ZERO_OPERANDS 0
#define ONE_OPERAND 1
#define TWO_OPERANDS 2
#define JUMP 3
#define LOAD 4
#define STORE 5
#define PUSH 6
#define POP 7

// Vrednosti koje yylex vraca kad matchuje adresiranje
enum AddressingModes {
    IMMED = 0, REGDIR, REGIND, REGINDREL, MEM, REGDIRREL, PCREL, J_MEM, ABS
};

enum PCFlag {
    PCREL_SET,
    PCREL_SAME_SECTION,
    PCREL_UNDEF,
    PCREL_NOTSET
};


FILE* out;

/*
*   Funkcija u kojoj se vrsi asembliranje.
*   Poziva se iz main.
*/
void assemble(void);

uint16_t processLiteral2(char*);
bool isLiteral(char*);

#endif