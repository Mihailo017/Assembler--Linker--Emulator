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

void int_i() {
    unsigned char regsDesc = memory[pc++];
    int dst = (regsDesc & 0xF0) >> 4;
    functionPush(cpu->psw);
    pc = memory[(cpu->r[dst]%8)*2];
}

short temp;
void arithLogic(int opCode, int size) {

    unsigned char regsDesc = memory[pc++];
    int dst = (regsDesc & 0xF0) >> 4;
    int src = regsDesc & 0x0F;

    operation(opCode, dst, src);

    setFlags(opCode, cpu->r[dst], cpu->r[src], temp);
}

void getJumpsOperand(int addressing, int update, int src, unsigned short* address, short* value) {
    switch (addressing) {
        case IMMED: {
            *value = memory[pc++];
            *value |= memory[pc++] << 8;
            break;
        }
        case REGDIR: {
            *value = cpu->r[src];
            break;
        }
        case REGIND: {
            if (update == 1) cpu->r[src] -= 2;
            if (update == 2) cpu->r[src] += 2;
            *address = cpu->r[src];
            if (update == 3) cpu->r[src] -= 2;
            if (update == 4) cpu->r[src] += 2;
            *value = memory[*address];
            *value |= memory[*(address+1)] << 8;
            break;
        }
        case REGINDREL: {
            if (update == 1) cpu->r[src] -= 2;
            if (update == 2) cpu->r[src] += 2;
            *address = cpu->r[src];
            if (update == 3) cpu->r[src] -= 2;
            if (update == 4) cpu->r[src] += 2;
            unsigned short temp = memory[pc++] | (memory[pc++] << 8);
            *address += temp;
            *value = memory[*address];
            *value |= memory[*(address+1)] << 8;
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

void jump(int opCode) {

    unsigned char regsDesc = memory[pc++];
    int src = regsDesc & 0x0F;

    unsigned char addrMode = memory[pc++];
    int addressing = addrMode & 0x0F;
    int update = (addrMode & 0xF0) >> 4;
    unsigned short address;
    short value = 0;

    getJumpsOperand(addressing, update, src, &address, &value);

    if (addressing == PCREL && src == 7) {
        address += pc;
        value = memory[address];
        value |= memory[address+1] << 8;
    }

    switch (opCode) {
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
            if (!(cpu->psw & FLAG_Z)) pc = value;
            break;
        }
        case JGT: {
            if (cpu->psw & FLAG_N) pc = value;
            break;
        }
    }
}

void ldr() {
    unsigned char regsDesc = memory[pc++];
    int src = regsDesc & 0x0F;
    int dst = (regsDesc & 0xF0) >> 4;

    unsigned char addrMode = memory[pc++];
    int addressing = addrMode & 0x0F;
    int update = (addrMode & 0xF0) >> 4;
    unsigned short address;
    short value = 0;

    getJumpsOperand(addressing, update, src, &address, &value);

    if (addressing == PCREL && src == 7) {
        address += pc;
        value = memory[address];
        value |= memory[address+1] << 8;
    }

    cpu->r[dst] = value;
}

void str() {
    unsigned char regsDesc = memory[pc++];
    int src = regsDesc & 0x0F;
    int dst = (regsDesc & 0xF0) >> 4;

    unsigned char addrMode = memory[pc++];
    int addressing = addrMode & 0x0F;
    int update = (addrMode & 0xF0) >> 4;
    unsigned short address = 0;
    short value = 0;

    switch (addressing) {
        case REGDIR: {
            cpu->r[src] = cpu->r[dst];
            break;
        }
        case REGIND: {
            if (update == 1) cpu->r[src] -= 2;
            if (update == 2) cpu->r[src] += 2;
            address = cpu->r[src];
            if (update == 3) cpu->r[src] -= 2;
            if (update == 4) cpu->r[src] += 2;
            memory[address] = cpu->r[dst] & 0x00FF;
            memory[address+1] = (cpu->r[dst] & 0xFF00) >> 8;
            break;
        }
        case REGINDREL: {
            if (update == 1) cpu->r[src] -= 2;
            if (update == 2) cpu->r[src] += 2;
            address = cpu->r[src];
            if (update == 3) cpu->r[src] -= 2;
            if (update == 4) cpu->r[src] += 2;
            unsigned short temp = memory[pc++] | (memory[pc++] << 8);
            address += temp;
            memory[address] = cpu->r[dst] & 0x00FF;
            memory[address+1] = (cpu->r[dst] & 0xFF00) >> 8;
            break;
        }
        case MEM: {
            address = memory[pc++] | (memory[pc++] << 8);
            memory[address] = cpu->r[dst] & 0x00FF;
            memory[address+1] = (cpu->r[dst] & 0xFF00) >> 8;
            break;
        }
        case PCREL: {
            address = memory[pc++] | (memory[pc++] << 8);
            address += pc;
            memory[address] = cpu->r[dst] & 0x00FF;
            memory[address+1] = (cpu->r[dst] & 0xFF00) >> 8;
            break;
        }
    }

    if (address == DATA_OUT_TERMINAL) {
        dataOutFlag = 1;
    }
}

void operation(int opCode, int dst, int src) {
    switch (opCode) {
        case XCHG: {
            short temp = cpu->r[dst]; cpu->r[dst] = cpu->r[src]; cpu->r[src] = temp;
            break;
        }
        case ADD: cpu->r[dst] += cpu->r[src];  break;
        case SUB: cpu->r[dst] -= cpu->r[src];  break;
        case MUL: cpu->r[dst] *= cpu->r[src];  break;
        case DIV: cpu->r[dst] /= cpu->r[src];  break;
        case CMP: temp = cpu->r[dst] - cpu->r[src]; break;
        case NOT: cpu->r[dst] = ~cpu->r[dst];  break;
        case AND: cpu->r[dst] &= cpu->r[src];  break;
        case OR: cpu->r[dst] |= cpu->r[src];  break;
        case XOR: cpu->r[dst] ^= cpu->r[src];  break;
        case TEST: temp = cpu->r[dst] & cpu->r[src]; break;
        case SHL: cpu->r[dst] << cpu->r[src];  break;
        case SHR: cpu->r[dst] >> cpu->r[src];  break;
    }
}

void setFlags(int opCode, short value1, short value2, short result) {
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
        case TEST: {
            // Ne postavlja druge flegove
            break;
        }
        case CMP: { // Isto ko i sub
            if (value1 < 0 && value2 < 0 && result > 0) temp |= FLAG_O;
            else temp &= ~FLAG_O;
            if ((value2 - value1) & 0x10000) temp |= FLAG_C;
            else temp &= ~FLAG_C;
            break;
        }
    }
    cpu->psw = temp;
}