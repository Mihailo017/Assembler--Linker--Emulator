#include "assembler.h"

#include "rellocation.h"
#include "symtab.h"
#include "section.h"
#include "instruction.h"
#include "allocation.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>

extern char* yytext;
extern unsigned long yyleng;

static Symbol_h currSym = NULL;
Section_h currSec = NULL;
static Instruction_h currIns = NULL;
static uint16_t locationCounter = 0;
static bool eofReached = false;

List_Symbol_h symbols;

typedef List_h List_GlobalNames_h;
List_GlobalNames_h globalNamesList = NULL;

Section_h absSection = NULL;

uint16_t processLiteral2(char* name)
{
    char* literal;
    bool isHex = false;

    if (name[1] == 'x' || name[1] == 'X') {
        literal = &name[2];// Preskoci 0x ili 0X
        isHex = true;
    } else if (name[strlen(name)-1] == 'h') {
        name[strlen(name)-1] = '\0'; // Skini h sa kraja
        literal = name;
        isHex = true;
    } else {
        literal = name;
    }

    return isHex ? strtol(literal, NULL, 16) : atoi(literal);
}

bool isLiteral(char* l)
{
    return (l[0] >= '0' && l[0] <= '9');
}

static char* string_createCopy(char* src)
{
    char* dst = malloc(strlen(src));
    strcpy(dst, src);
    return dst;
}

static uint8_t charToNum(char reg)
{
    return reg - '0';
}


/*
*   yytext = .section;
*   Nakon poziva yylex: yytext = .ime_sekcije
*   Sekcija kao simbol se dodaje u tabelu i alocira se niz za njen sadrzaj.
*/
static void processSection2(void)
{
    yylex();
    currSym = list_symbol_find(symbols, yytext);

    if (currSym == NULL) {
        if (currSec) section_setSize(currSec, locationCounter);
        locationCounter = 0;
        currSym = currSec = section_create(yytext);
        list_addTail(sections, currSec);
    } else { // Nastavak sekcije
        section_setSize(currSec, locationCounter);
        currSec = currSym;
        locationCounter = section_getSize(currSec);
    }
}

/*
*   Simbol se dodaje u tabelu kao definisan.
*/
static void processLabel2(void)
{
    yytext[yyleng-1] = '\0'; // Skini dvotacku sa kraja
    currSym = createDefinedSymbol(yytext, 'L');
    list_addTail(symbols, currSym);
}

/*
*   Upis sadrzaja sekcije na nivou bajta ili dvobajtne reci.
*   Obrada dok se ne dodje do kraja liste simbola odvojenih zapetama.
*/

static void processWord()
{
    Allocation_h allocation = allocation_create(locationCounter);
    section_addAllocation(currSec, allocation);
    while (true) {

        uint8_t ret = yylex();

        if (ret == LISTED) yytext[yyleng-1] = '\0'; // Skidanje zapete sa kraja

        char* symbolName = string_createCopy(yytext);

        allocation_addName(allocation, symbolName);

        locationCounter += WORDSIZE;
        if (ret != LISTED) break;
    }
}

/*
*   Uvecanje velicine sekcije.
*   Argument je uvek literal.
*/
static void processSkip(void)
{
    yylex();
    locationCounter += processLiteral2(yytext);
}


void processEqu3(void)
{

    yylex();
    yytext[yyleng-1] = '\0'; // Skidanje zapete sa kraja
    char* name = string_createCopy(yytext);

    yylex();
    if (isLiteral(yytext)) {
        int16_t offset = processLiteral2(yytext);
        Symbol_h s = createAbsoluteSymbol(name, offset);
        // if (absSection == NULL) {
        //     absSection = section_create(".abs");
        //     list_addTail(sections, absSection);
        // }
        // symbol_setSection(s, absSection);
        list_addTail(symbols, s);
    }
}


static void processGlobal2(void)
{
    while (true) {
        int ret = yylex();

        if (ret == LISTED) yytext[yyleng-1] = '\0'; // Skidanje zapete sa kraja
        list_addTail(globalNamesList, string_createCopy(yytext));

        if (ret != LISTED) break;
    }
}

static void processExtern2(void)
{
    while (true) {
        int ret = yylex();

        if (ret == LISTED) yytext[yyleng-1] = '\0';
        if (list_symbol_find(symbols, yytext) == NULL) {
            currSym = createExternSymbol(yytext);
            list_addTail(symbols, currSym);

            if (ret != LISTED) break;
        } else {
            printf("Naknadno definisan eksterni simbol!\n");
            exit(1);
        }
    }
}

#define FIRST_OPERAND 0
#define SECOND_OPERAND 1

static void processAdressing2(uint8_t addressing, uint8_t operandOrd)
{
    uint8_t insDesc = instruction_getInstructionDescriptor(currIns);

    if (addressing == J_MEM) addressing = MEM;

    if (insDesc == STORE && addressing == IMMED && operandOrd == SECOND_OPERAND) {
        printf("Ne moze se koristi IMMED za destinacioni operand STORE instrukcije!");
        exit(1);
    }

    uint8_t addressingBits = addressing;
    if (addressing == PCREL) addressingBits = REGDIRREL;
    if (insDesc == JUMP || operandOrd == SECOND_OPERAND)
        instruction_setAddressingMode(currIns, addressingBits);

    if (insDesc == JUMP && addressing != PCREL && addressing != IMMED)
        yytext = &yytext[1]; // Skidanje * sa pocetka   

    if (insDesc == JUMP && addressing == IMMED) addressing = ABS;
    
    char* name = NULL;
    switch(addressing) {
        case IMMED: name = &yytext[1]; break; // Da se ne uzme $ sa pocetka
        case REGDIR: {
            // yytext = rN gde je N broj reg pa zato 1
            if (insDesc == JUMP || operandOrd == SECOND_OPERAND)
                instruction_setRegSrc(currIns, charToNum(yytext[1]));
            else
                instruction_setRegDst(currIns, charToNum(yytext[1]));
            break;
        }
        case REGIND: {
            // yytext = [rN] gde je N broj reg pa zato 2
            if (insDesc == JUMP || operandOrd == SECOND_OPERAND)
                instruction_setRegSrc(currIns, charToNum(yytext[2]));
            else
                instruction_setRegDst(currIns, charToNum(yytext[2]));
            break;
        }
        case REGINDREL: {
            int i;
            // Od pocetka do plusa je oznaka registra, dalje je ime simbola
            for (i=0; i<yyleng; i++) if (yytext[i] == '+') break;
            yytext[i] = '\0';
            name = yytext + i + 1;
            // Beli znakovi
            for (i=0; i<yyleng; i++) if (name[i] != ' ') break;  
            // Skidanje ] sa kraja
            for (i=0; i<yyleng; i++) if (name[i] == ']') break;
            name[i] = '\0';
            name = name + (i - 1);
            // yytext = [rN] gde je N broj reg pa zato 2
            uint8_t reg = charToNum(yytext[2]);
            if (insDesc == JUMP || operandOrd == SECOND_OPERAND)
                instruction_setRegSrc(currIns, reg);
            else
                instruction_setRegDst(currIns, reg);
            break;
        }
        case MEM: name = yytext; if (operandOrd == FIRST_OPERAND && name[strlen(name)-1] == ',') yytext[yyleng-1] = '\0'; break; // Skida se zarez ako ga ima jer ga hvata kao EXPR_SYMBOL
        case PCREL: {
            name = &yytext[1]; // Da se ne uzme % sa pocetka
            uint8_t pc = 7;
            if (insDesc == JUMP || operandOrd == SECOND_OPERAND)
                instruction_setRegSrc(currIns, pc);
            else
                instruction_setRegDst(currIns, pc);
            break;
        }
        case ABS: name = yytext;
    }

    if (name != NULL)
        instruction_setOperandName(currIns, name);
}

static void processOperandIns3(uint8_t operandOrd)
{
    uint8_t addressing = yylex();
    uint8_t insDesc = instruction_getInstructionDescriptor(currIns);

    if ((insDesc == PUSH || insDesc == POP) && addressing != REGDIR) {
        printf("Za push i pop se mora koristiti registarsko direktno adresiranje!");
        exit(1);
    }

    processAdressing2(addressing, operandOrd);

    return;
}

enum PCFlag pcRelFlag[2] = {PCREL_NOTSET, PCREL_NOTSET};

static void processInstruction2(uint8_t instructionDesc) {

    currIns = instruction_create(yytext, instructionDesc, locationCounter, currSec);

    if (instructionDesc == ONE_OPERAND || instructionDesc == TWO_OPERANDS) {
        yylex();
        uint8_t dst = charToNum(yytext[1]), src = 0xF;
        if (instructionDesc == TWO_OPERANDS) {
            yylex();
            src = charToNum(yytext[1]);
        }
        instruction_setRegsDescr(currIns, dst, src);
    }

    if (instructionDesc == JUMP || instructionDesc == LOAD || instructionDesc == STORE) {
        processOperandIns3(0);
        if (instructionDesc == LOAD || instructionDesc == STORE)
            processOperandIns3(1);
    }

    if (instructionDesc == PUSH || instructionDesc == POP) {
        uint8_t src = 0x6;
        processOperandIns3(0);
        instruction_setRegSrc(currIns, src);
        if (instructionDesc == PUSH) {
            instruction_setAddressingMode(currIns, 0x12);    
        } else {
            instruction_setAddressingMode(currIns, 0x42);
        }
        
    }

    section_addInstruction(currSec, currIns);

    locationCounter += instruction_getSize(currIns);
}

static void firstPass(void)
{
    while(!eofReached) {
        int8_t temp = yylex();
        switch(currentlyProcessing) {
            case SECTION: processSection2(); break;

            case LABEL: processLabel2(); break;

            case ALLOCC_WORD: processWord(); break;

            case SKIP: processSkip(); break;

            case GLOBAL: processGlobal2(); break;

            case EXTERN: processExtern2(); break;

            case INSTRUCTION_JUMP:
            case INSTRUCTION: processInstruction2(temp); break;

            case EQU: processEqu3(); break;

            case EOF_REACHED : {
                section_setSize(currSec, locationCounter);
                eofReached = true;
                break;
            }
            
            case INITIAL_VALUE: break;
            case COMMENT: break;
        }
    }
}

static void processGobalList()
{
    for (list_start(globalNamesList); list_notEnd(globalNamesList); list_next(globalNamesList)) {
        char* name = (char*)list_get(globalNamesList);
        Symbol_h s = list_symbol_find(symbols, name);
        if (s != NULL)
            symbol_setLinking(s, 'G');
        else {
            printf("Simbol naveden nakon .global nije definisan!");
            exit(1);
        }
    }
}

static void secondPass(void)
{   
    for (list_start(sections); list_notEnd(sections); list_next(sections)) {
        Symbol_h section = (Symbol_h)list_get(sections);
        section_secondPass(section);
    }

    for (list_start(sections); list_notEnd(sections); list_next(sections)) {
        Symbol_h section = (Symbol_h)list_get(sections);
        section_writeContent(section);
    }
    fprintf(out, "%c", '#');
}

void assemble(void)
{
    list_create(&symbols, NULL);
    list_create(&globalNamesList, NULL);
    list_create(&sections, section_delete);

    firstPass();

    processGobalList();

    list_symbol_write(symbols);

    secondPass();

    list_delete(symbols);
    list_delete(globalNamesList);
    list_delete(sections);
}


