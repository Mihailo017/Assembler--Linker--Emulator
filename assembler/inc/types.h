#include "list.h"

#include <stdint.h>

typedef List List_Symbol;
typedef List_h List_Symbol_h;

typedef uint64_t Symbol_id;
typedef Symbol_id* Symbol_h;

typedef List_h List_Sections_h;

typedef uint64_t Section_id;
typedef Section_id* Section_h;

typedef List InstructionList;
typedef List_h List_Instruction_h;

typedef uint64_t Instruction_id;
typedef Instruction_id* Instruction_h;

typedef List List_SymbolNames;
typedef List_h List_SymbolNames_h;

typedef List List_Allocation;
typedef List_h List_Allocation_h;

typedef uint64_t Allocation_id;
typedef Allocation_id* Allocation_h;

typedef List_h List_Rellocation_h;

typedef uint64_t Rellocation_id;
typedef Rellocation_id* Rellocation_h;