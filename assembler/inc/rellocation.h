#ifndef _RELLOCATION_H_
#define _RELLOCATION_H_

#include "list.h"
#include "symtab.h"
#include "types.h"

// tipovi relokacija
typedef enum Rellocation_Type {R_16, R_PC16} Rellocation_Type;
#define S_R_16 "R_16"
#define S_R_PC16 "R_PC16"

typedef struct Rellocation {
    Rellocation_id id;
    Symbol_h symbol;
    uint16_t offset;
    Rellocation_Type type;
} Rellocation;

Rellocation_h rellocation_create(Symbol_h symbol, uint16_t offset, uint8_t addressing);

void rellocation_print(Rellocation_h rellocation);
void rellocation_write(Rellocation_h rellocation);

Symbol_h rellocation_getSymbol(Rellocation_h r);

void typeToString(Rellocation*);
void typeToFILE(Rellocation*);

void rellocation_print(Rellocation_h);
void rellocation_write();

#endif