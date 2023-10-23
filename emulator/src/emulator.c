#include "emulator.h"
#include "processor.h"
#include "instruction.h"

#include <stdlib.h>
#include <string.h>

extern FILE* in;
static char data[8];
static void parseHex(void) {
    char *line = malloc(LINE_LENGTH * sizeof(char));
    unsigned long int length = LINE_LENGTH;
    getline(&line, &length, in);
    char* format = malloc(sizeof(char) * 100);
    strcat(format, "%x\t: ");
    for (int i=0; i<8; i++) {
        strcat(format, "%x ");
    }
    int start;
    textSectionStart = -1;
    getline(&line, &length, in);
    while (strcmp(line, "\n") != 0) {
        sscanf(line, format, &start, &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7]);
        memcpy(memory+start, data, 8);
        if (textSectionStart == -1) textSectionStart = start;
        getline(&line, &length, in);
    }
    
}

void executeInstructions() {

    initializeProc();

    while (!halted) {
        unsigned char opcode = memory[pc++];
        if (opcode >= XCHG && opcode <= SHR) {
            arithLogic(opcode, 2);
        } else {
            if (opcode == CALL || (opcode >= JMP && opcode <= JGT)) {
                jump(opcode);
            } else {
                switch(opcode) {
                    case HALT: halt(); break;
                    case INT:  int_i(); break;
                    case IRET: iret(); break;
                    case RET: ret(); break;
                    case LDR: ldr(); break;
                    case STR: str();break;
                }
            }
        }
        if (dataOutFlag) writeTerminal();
        proccessInterrupts();
        processInterrupts2();
    }
}

void emulate(void) {

    parseHex();
    executeInstructions();

}