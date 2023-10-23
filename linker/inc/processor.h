#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <time.h>
#include <pthread.h>

#define ADD_UNIT 1
#define WORDLENGTH 2
#define MEMORY_SIZE 65535
#define MEMORY_MAPPED_REG_START 0xFF
#define MEMORY_MAPPED_REG_SIZE 256

#define DATA_OUT_TERMINAL 0xFF00
#define DATA_IN_TERMINAL 0xFF02
#define TIMER_CFG 0xFF10

#define STACK_START 0xFF00
#define STACK_SIZE 1000 // U bajtovima

#define IVT_START 0x0000
#define IVT_NUM_ENTRIES 8
#define IVT_ENTRY_SIZE 2
#define INTERUPTS 4
#define TIMER 2
#define TERMINAL 3

#define GLOBAL_START 0x0010
#define GLOBAL_NUM 100 // Broj simbola
#define GLOBAL_END 0x00D8 // Prostor za 100 globalnih simbola 

#define pc cpu->r[7]
#define sp cpu->r[6]
#define REGISTERS 15

#define FLAG_I 0x8000
#define FLAG_TL 0x4000
#define FLAG_TR 0x2000

#define FLAG_N 0x0008
#define FLAG_C 0x0004
#define FLAG_O 0x0002
#define FLAG_Z 0x0001

int dataOutFlag;
clock_t timer;
int trigger;

pthread_t terminalThread;

typedef struct Processor {
    short r[REGISTERS];
    short psw;
} Processor;

Processor* cpu;
unsigned char memory[MEMORY_SIZE];

void initializeProc();

void functionPush(short value);
short functionPop();

int interrupts[INTERUPTS];

void checkTimer();
void writeTerminal();

void proccessInterrupts();

void processInterrupts2();


#endif