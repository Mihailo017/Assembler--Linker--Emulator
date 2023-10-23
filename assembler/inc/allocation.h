#ifndef _ALLOCATION_H_
#define _ALLOCATION_H_

#include <stdint.h>
#include "list.h"
#include "symtab.h"

typedef struct Allocation {
    Allocation_id id;
    List_SymbolNames_h symbolNameList;
    uint16_t start;
    uint16_t size;
} Allocation;

Allocation_h allocation_create(uint16_t start);
void allocation_addAllocation(List_Allocation_h, uint16_t);
void allocation_addName(Allocation_h, char*);
int8_t* allocation_getBytes(Allocation_h, Section_h);
uint16_t allocation_getSize(Allocation_h);
uint16_t allocation_getOffset(Allocation_h);

#define createAllocationList(handle) createList2(&handle ,  NULL)
#define freeAllocationList(handle) emptyList2(handle);

#endif