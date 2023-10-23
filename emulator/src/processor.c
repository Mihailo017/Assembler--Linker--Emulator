#include "processor.h"
#include "emulator.h"
#include "terminal.h"


#include <stdlib.h>
#include <string.h>

void initializeProc() {
    halted = 0;
    cpu = calloc(1, sizeof(Processor));
    pc = textSectionStart;

    pc = memory[0] | (memory[1] << 8);

    for (int i=0; i < INTERUPTS; i++) interrupts[i] = 0;

    sp = STACK_START-1;
    cpu->psw = 0;
    dataOutFlag = 0;
    trigger = 500;
    timer = clock();

    if (pthread_create(&terminalThread, 0, inTerm, 0)) {
        perror(NULL); exit(1);
    } 
}

void functionPush(short value) {
    memory[(unsigned short)(--sp)] = (value & 0xFF00) >> 8;
    memory[(unsigned short)(--sp)] = value & 0xFF;
}

short functionPop() {
    short ret;
    ret = memory[(unsigned short)(sp++)]; // Nizi
    ret |= memory[(unsigned short)(sp++)] << 8; // Visi
    return ret;
}

void checkTimer() {
    clock_t diff = clock() - timer;
    int msecs = diff * 1000 / CLOCKS_PER_SEC;
    if (msecs > trigger) {
        interrupts[TIMER] = 1;
        timer = clock();
        switch(memory[TIMER_CFG]) {
        case 0: trigger = 500; break;
        case 1: trigger = 1000; break;
        case 2: trigger = 1500; break;
        case 3: trigger = 2000; break;
        case 4: trigger = 5000; break;
        case 5: trigger = 10000; break;
        case 6: trigger = 30000; break;
        case 7: trigger = 60000; break;
        }
    }
}

void writeTerminal() {
    unsigned short adr = DATA_OUT_TERMINAL;
    printf("%c \n", (char)memory[adr]);
    dataOutFlag = 0;
}

void proccessInterrupts() {
    checkTimer();
}

void processInterrupts2() {
for (int i = 0; (i < INTERUPTS) && !(cpu->psw & FLAG_I); i++) {
        if (interrupts[i]) {
            if (i == TIMER && (cpu->psw & FLAG_TR)) continue;
            if (i == TERMINAL && (cpu->psw & FLAG_TL)) continue;
            interrupts[i] = 0;
            if (memory[i * IVT_ENTRY_SIZE] != 0) {
                functionPush(pc);
                functionPush(cpu->psw);
                pc = memory[i * IVT_ENTRY_SIZE];
                pc |= memory[i * IVT_ENTRY_SIZE+1] << 8;
            }
        }
    }
}