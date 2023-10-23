#include "rellocation.h"
#include "assembler.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

extern Symbol_h currSym;
extern Symbol_h currSec;

typedef List RellocationList;
typedef List_h RListHandle;
static RListHandle rellocList = NULL;

Rellocation_h rellocation_create(Symbol_h symbol, uint16_t offset, uint8_t addressing)
{
    Rellocation *r = malloc(sizeof(Rellocation));
    r->offset = offset;
    r->symbol = symbol;
    r->type = addressing == PCREL ? R_PC16 : R_16;
    return (Rellocation_h)r;
}

void typeToString(Rellocation* relloc) {
    switch(relloc->type) {
        case R_16: printf(S_R_16); break;
        case R_PC16: printf(S_R_PC16); break;
    }
    printf("\t");
}

void typeToFILE(Rellocation* relloc) {
    switch(relloc->type) {
        case R_16: fprintf(out, S_R_16); break;
        case R_PC16: fprintf(out, S_R_PC16); break;
    }
    fprintf(out, "\t");
}

void rellocation_print(Rellocation_h rellocation)
{
    Rellocation* r = (Rellocation*)rellocation;
    printf("%0*x\t\t", 4, r->offset);
    r->type == R_16 ? printf(S_R_16) : printf(S_R_PC16);
    printf("\t");
    symbol_getLinking(r->symbol) == 'L' ? printf("%" PRIu64 "\n", *r->symbol) : printf("%" PRIu64 "\n", *symbol_getSection(r->symbol));
}

extern FILE* out;
void rellocation_write(Rellocation_h rellocation)
{
    Rellocation* r = (Rellocation*)rellocation;
    fprintf(out, "%0*x\t\t", 4, r->offset);
    r->type == R_16 ? fprintf(out, S_R_16) : fprintf(out, S_R_PC16);
    fprintf(out, "\t");
    symbol_getLinking(r->symbol) != 'L' ? fprintf(out, "%" PRIu64 "\n", *r->symbol) : fprintf(out, "%" PRIu64 "\n", *symbol_getSection(r->symbol));
}