#include "allocation.h"

#include "section.h"
#include "assembler.h"
#include "rellocation.h"

#include <string.h>
#include <stdlib.h>

static uint64_t nextId = 0;

Allocation_h allocation_create(uint16_t start)
{
    Allocation* a = malloc(sizeof(Allocation));
    a->id = ++nextId;
    list_create(&a->symbolNameList, NULL);
    a->start = start;
    a->size = 0;
    return (Allocation_h)a;
}

void allocation_addAllocation(List_Allocation_h allocList, uint16_t start)
{
    if (!allocList)
        list_create(&allocList, NULL);
    list_addHead(allocList, allocation_create(start));
}

void allocation_addName(Allocation_h allocation, char* name)
{
    Allocation* a = (Allocation*)allocation;
    a->size += 2;
    list_addTail(a->symbolNameList, name);
}

extern List_Symbol_h symbols;
int8_t* allocation_getBytes(Allocation_h al, Section_h s)
{
    Allocation* a = (Allocation*)al;
    uint16_t i = 0;
    int8_t* bytes = malloc(a->size);
    for (list_start(a->symbolNameList); list_notEnd(a->symbolNameList); list_next(a->symbolNameList)) {
        char* name = (char*)list_get(a->symbolNameList);
        uint16_t offset = 0;
        if (isLiteral(name)) {
            offset = processLiteral2(name);
        } else {
            Symbol_h symbol = list_symbol_find(symbols, name);
            offset = symbol_getOffset(symbol);
            Rellocation_h rellocation = rellocation_create(symbol, a->start+i, !PCREL);
            section_addRellocation(s, rellocation);          
        }
        bytes[i++] = offset & 0xFF;
        bytes[i++] = (offset & 0xFF00) >> 8;
    }
    return bytes;
}

uint16_t allocation_getSize(Allocation_h a)
{
    Allocation* allocation = (Allocation*)a;
    return allocation->size;    
}

uint16_t allocation_getOffset(Allocation_h a)
{
    Allocation* allocation = (Allocation*)a;
    return allocation->start;  
}