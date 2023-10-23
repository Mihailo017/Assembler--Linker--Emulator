#include "instruction.h"
#include "processor.h"
#include "emulator.h"

#include <stdlib.h>

void halt() {
    halted = 1;
}

void iret() {
    cpu->psw = functionPop();
    pc = functionPop();
}

void ret() {
    pc = functionPop();
}

void arithLogic(int opCode, int size) {


    unsigned char opDesc[2] = {0};
    int addressing[2] = {0};
    int regN[2] = {0};
    int lh[2] = {0};
    unsigned short address[2] = {0};
    short value[2] = {0};

    for (int i=0; i < 2; i++) {
        opDesc[i] = memory[pc++];
        addressing[i] = opDesc[i] >> 5;
        regN[i] = (opDesc[i] & 0x1E) >> 1;
        lh[i] = opDesc[i] & 0x1;

        extractValues(size, addressing[i], regN[i], lh[i], &address[i], &value[i]);
    }

    /* PC moze da se doda tek sad jer tek nakon citanja oba operanda
    *   on zapravo pokazuje na narednu instrukciju
    */
    for (int i=0; i < 2; i++) {    
        if (addressing[i] == REGINDREL && regN[i] == 7) {
            address[i] += pc;
            value[i] = memory[address[i]];
            value[i] |= memory[address[i]+1] << 8;
        }
    }

    if (address[1] == DATA_OUT_TERMINAL) {
        dataOutFlag = 1;
    }

    short result = operation(opCode, value[FIRST_OPERAND], value[SECOND_OPERAND]);

    if (opCode != TEST && opCode != CMP) {
        storeToDst(addressing[SECOND_OPERAND], regN[SECOND_OPERAND],
            address[SECOND_OPERAND], result);
    }

    setFlags(opCode, value[FIRST_OPERAND], value[SECOND_OPERAND], result);
}

void jump(int opCode) {
    unsigned char opDesc = 0;
    int addressing = 0;
    int regN = 0;
    int lh = 0;
    unsigned short address = 0;
    short value = 0;

    opDesc = memory[pc++];
    addressing = opDesc >> 5;
    regN = (opDesc & 0x1E) >> 1;
    lh = opDesc & 0x1;

    extractValues(2, addressing, regN, lh, &address, &value);

    if (addressing == REGINDREL && regN == 7) {
        address += pc;
        value = memory[address];
        value |= memory[address+1] << 8;
    }

    switch (opCode) {
        case INT: {
            functionPush(cpu->psw);
            pc = memory[(value%8)*2];
            break;
        }
        case CALL: {
            functionPush(pc);
            pc = value;
            break;
        }
        case JMP: {
            pc = value;
            break;
        }
        case JEQ: {
            if (cpu->psw & FLAG_Z) pc = value;
            break;
        }
        case JNE: {
            //printf("JNE %d\n", value);
            if (!(cpu->psw & FLAG_Z)) pc = value;
            break;
        }
        case JGT: {
            if (cpu->psw & FLAG_N) pc = value;
            break;
        }
    }
}

void push() {
    unsigned char opDesc = 0;
    int addressing = 0;
    int regN = 0;
    int lh = 0;
    unsigned short address = 0;
    short value = 0;

    opDesc = memory[pc++];
    addressing = opDesc >> 5;
    regN = (opDesc & 0x1E) >> 1;
    lh = opDesc & 0x1;

    extractValues(2, addressing, regN, lh, &address, &value);

    if (addressing == REGINDREL && regN == 7) {
        address += pc;
        value = memory[address];
        value |= memory[address+1] << 8;
    }

    functionPush(value);
}

void pop() {
    unsigned char opDesc = 0;
    int addressing = 0;
    int regN = 0;
    int lh = 0;
    unsigned short address = 0;
    short value = 0;

    opDesc = memory[pc++];
    addressing = opDesc >> 5;
    regN = (opDesc & 0x1E) >> 1;
    lh = opDesc & 0x1;

    extractValues(2, addressing, regN, lh, &address, &value);

    if (addressing == REGINDREL && regN == 7) {
        address += pc;
        value = memory[address];
        value |= memory[address+1] << 8;
    }

    value = functionPop();
}

void xchg() {

    unsigned char opDesc[2] = {0};
    int addressing[2] = {0};
    int regN[2] = {0};
    int lh[2] = {0};
    unsigned short address[2] = {0};
    short value[2] = {0};

    for (int i=0; i < 2; i++) {
        opDesc[i] = memory[pc++];
        addressing[i] = opDesc[i] >> 5;
        regN[i] = (opDesc[i] & 0x1E) >> 1;
        lh[i] = opDesc[i] & 0x1;

        extractValues(2, addressing[i], regN[i], lh[i], &address[i], &value[i]);
    }

    /* PC moze da se doda tek sad jer tek nakon citanja oba operanda
    *   on zapravo pokazuje na narednu instrukciju
    */
    for (int i=0; i < 2; i++) {    
        if (addressing[i] == REGINDREL && regN[i] == 7) {
            address[i] += pc;
            value[i] = memory[address[i]];
            value[i] |= memory[address[i]+1] << 8;
        }
    }

    if (address[1] == DATA_OUT_TERMINAL) {
        dataOutFlag = 1;
    }

    storeToDst(addressing[SECOND_OPERAND], regN[SECOND_OPERAND],
        address[SECOND_OPERAND], value[FIRST_OPERAND]);
    storeToDst(addressing[FIRST_OPERAND], regN[FIRST_OPERAND],
        address[FIRST_OPERAND], value[SECOND_OPERAND]);
}

void mov() {
    unsigned char opDesc[2] = {0};
    int addressing[2] = {0};
    int regN[2] = {0};
    int lh[2] = {0};
    unsigned short address[2] = {0};
    short value[2] = {0};

    for (int i=0; i < 2; i++) {
        opDesc[i] = memory[pc++];
        addressing[i] = opDesc[i] >> 5;
        regN[i] = (opDesc[i] & 0x1E) >> 1;
        lh[i] = opDesc[i] & 0x1;

        extractValues(2, addressing[i], regN[i], lh[i], &address[i], &value[i]);
    }

    //printf("MOV %d %d\n", value[0], value[1]);

    /* PC moze da se doda tek sad jer tek nakon citanja oba operanda
    *   on zapravo pokazuje na narednu instrukciju
    */
    for (int i=0; i < 2; i++) {    
        if (addressing[i] == REGINDREL && regN[i] == 7) {
            address[i] += pc;
            value[i] = memory[address[i]];
            value[i] |= memory[address[i]+1] << 8;
        }
    }

    if (address[1] == DATA_OUT_TERMINAL) {
        dataOutFlag = 1;
    }
    
    short result = value[FIRST_OPERAND];

    storeToDst(addressing[SECOND_OPERAND], regN[SECOND_OPERAND],
        address[SECOND_OPERAND], result);

    setFlags(MOV, value[FIRST_OPERAND], value[SECOND_OPERAND], result);
}

short operation(int opCode, short value1, short value2) {
    switch (opCode) {
        case ADD: /*printf("ADD %d %d\n", value1, value2);*/ return value2 + value1;  break;
        case SUB: /*printf("SUB %d %d\n", value1, value2);*/ return value2 - value1; break;
        case MUL: return value2 * value1; break;
        case DIV: /*printf("DIV %d %d\n", value1, value2);*/ return value2 / value1; break;
        case CMP: return value2 - value1; break;
        case NOT: return ~value1; break;
        case AND: return value2 & value1; break;
        case OR: return value2 | value1; break;
        case XOR: return value2 ^ value1; break;
        case TEST: return value2 & value1; break;
        case SHL: return value2 << value1;  break;
        case SHR: return value2 >> value1;  break;
    }
}


void extractValues(int size, int addressing, int regN, int lh, unsigned short* address, short* value) {
    switch (addressing) {
        case IMMED: {
            *value = memory[pc++];
            *value |= memory[pc++] << 8;
            break;
        }
        case REGDIR: {
            if (lh && size == 1) *value = cpu->r[regN] >> 8;
            else if (!lh && size == 1) *value = cpu->r[regN] & 0xFF;
            else *value = cpu->r[regN]; 
            break;
        }
        case REGIND: {
            *address = cpu->r[regN];
            *value = memory[*address];
            *value |= memory[*(address+1)] << 8;
            break;
        }
        case REGINDREL: {
            if (regN == 7) { // PCREL
                *address = 0;
            } else {
                *address = cpu->r[regN];
            }
            unsigned short temp = memory[pc++] | (memory[pc++] << 8);
            *address += temp;
            if (regN != 7) { // NOT PCREL
                *value = memory[*address];
                *value |= memory[*(address+1)] << 8;
            }
            break;
        }
        case MEM: {
            *address = 0;
            unsigned short temp = memory[pc++] | (memory[pc++] << 8);
            *address = temp;
            *value = memory[*address];
            *value |= memory[*address+1] << 8;
            break;
        }   
    }
}

void storeToDst(int addressing, int regN, unsigned short address, short value) {
    switch (addressing) {
        case IMMED: {
            printf("Ne moze se koristiti neposredno adresiranje za destinacioni operand!\nKrajprograma\n");
            exit(1);
        }
        case REGDIR: {
            cpu->r[regN] = value;
            break;
        }
        default: {
            memory[(unsigned short)address] = value & 0xFF;
            memory[address+1] = (value & 0xFF00) >> 8;
            break;
        }
    }
}

void setFlags(int opCode, short value1, short value2, short result) {
    if (opCode >= MOV) {
        short temp = cpu->psw;
        temp = result == 0 ? temp | FLAG_Z : temp & ~FLAG_Z;
        temp = result < 0 ? temp | FLAG_N : temp & ~FLAG_N;
        switch (opCode) {
            case SHL : {
                if (value2 & (0x8000 >> value1-1)) temp |= FLAG_C;
                else temp &= ~FLAG_C;
                break;
            }
            case SHR : {
                if (value2 & (0x1 << value1-1)) temp |= FLAG_C;
                else temp &= ~FLAG_C;
                break;
            }
            case ADD: {
                if (value1 > 0 && value2 > 0 && result < 0) temp |= FLAG_O;
                else temp &= ~FLAG_O;
                if ((value1 + value2) & 0x10000) temp |= FLAG_C;
                else temp &= ~FLAG_C;
                break;
            }
            case SUB: {
                if (value1 < 0 && value2 < 0 && result > 0) temp |= FLAG_O;
                else temp &= ~FLAG_O;
                if ((value2 - value1) & 0x10000) temp |= FLAG_C;
                else temp &= ~FLAG_C;
                break;
            }
            case CMP: { // Isto ko i sub
                if (value1 < 0 && value2 < 0 && result > 0) temp |= FLAG_O;
                else temp &= ~FLAG_O;
                if ((value2 - value1) & 0x10000) temp |= FLAG_C;
                else temp &= ~FLAG_C;
                break;
            }
            default: break;
        }
        cpu->psw = temp;
    }
}