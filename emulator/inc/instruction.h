#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

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

#define IMMED 0
#define REGDIR 1
#define REGIND 2
#define REGINDREL 3
#define MEM 4
#define PCREL 5

#define FIRST_OPERAND 0
#define SECOND_OPERAND 1

void halt();
void iret();
void ret();
void int_i();
void arithLogic(int opCode, int size);
void jump(int opCode);
void push();
void pop();
void xchg();
void ldr();
void str();
void mov();
void operation(int, int, int);

void extractValues(int size, int addressing, int regN, int lh, unsigned short* address, short* value);
void storeToDst(int addressing, int regN, unsigned short address, short value);
void setFlags(int opCode, short value1, short value2, short result);

#endif