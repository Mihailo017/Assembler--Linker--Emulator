#include "linker.h"
#include "symbol.h"
#include "section.h"
#include "processor.h"
#include "rellocation.h"
#include <stdlib.h>
#include <string.h>

int lastSectionId = 0;

static void writeHex() {
    int prevStart = 0;
    for (int i=0; i<sectionIndex; i++) {
        if (sections[i]->size > 0) {
            int start = sections[i]->start - (sections[i]->start % 8);
            if (start == prevStart && prevStart > 0) start += 8;
            int end = start + sections[i]->size;
            while (end % 8 != 0) end++;
            for (int j=0; start+j < end; j++) {
                if (j % 8 == 0) fprintf(out, "\n%0*x\t: ", 4, start+j);
                fprintf(out, "%0*x ", 2, memory[start+j]);
            }
            prevStart = end - 8;
        }
        for (int j=i+1; j<sectionIndex; j++) {
            if (strcmp(sections[j]->name, sections[i]->name) && sections[j]->size > 0) {
                int start = sections[i]->start - (sections[i]->start % 8);
                if (start == prevStart && prevStart > 0) start += 8;
                int end = start + sections[i]->size;
                while (end % 8 != 0) end++;
                for (int j=0; start+j < end; j++) {
                if (j % 8 == 0) fprintf(out, "\n%0*x\t: ", 4, start+j);
                fprintf(out, "%0*x ", 2, memory[start+j]);
                }
                prevStart = end - 8;
                
            }
        }
    }
    fprintf(out, "\n");
}

static void writeHex2() {
    int prevStart = 0;
    for (int i=0; i<sectionIndex; i++) {
        if (sections[i]->size > 0) {
            int start = sections[i]->start - (sections[i]->start % 8);
            if (start == prevStart && prevStart > 0) start += 8;
            int end = start + sections[i]->size;
            while (end % 8 != 0) end++;
            for (int j=0; start+j < end; j++) {
                if (j % 8 == 0) fprintf(out, "\n%0*x\t: ", 4, start+j);
                fprintf(out, "%0*x ", 2, memory[start+j]);
            }
            prevStart = end - 8;
        }
    }
    fprintf(out, "\n");
}

static void writeLinkable() {
    for (int i=0; i<sectionIndex; i++) {
        if (sections[i]->size > 0) {
            int toWrite = sections[i]->size > 25 ? 25 : sections[i]->size;
            fseek(out, sections[i]->positionInFile, SEEK_SET);
            for (int j = 0; j < toWrite; j++) {
                if (j % 25 == 0 && j) fprintf(out, "\n");
                fprintf(out, "%0*x ", 2, memory[sections[i]->start+j]);
            }
        }
    }
}

void initialize() {
    halted = 0;
    symbolTable = createSymbolTable();
    rellocationReccords = createRellocationTable();
    out = fopen("izlaz", "w");

    readSectionTable();

    readSymbolTable();

    readRellocationReccords();
}

void loadSections() {
    for (int i=0; i < inputFileIndex; i++) {

        while (strlen(line[i]) != 1) { // Dok ne dodje do # na kraju fajla
            /* Linija sa imenom sekcije je vec procitana samo ga izvuci*/
            char* temp = malloc(sizeof(char) * SECTION_NAME_LENGTH);
            sscanf(line[i], "# %s\n", temp);

            Section* section = NULL;
            for (int j=0; j < sectionIndex; j++) {
                if (strcmp(sections[j]->name, temp) == 0 && sections[j]->file == i) {
                    section = sections[j];
                    break;
                }
            }

            free(temp);

            //section->positionInFile = ftell(inputFiles[i]);

            ssize_t length;
            getline(&line[i], &length, inputFiles[i]);

            ssize_t leftToWrite = section->size;
            ssize_t writingPos = section->start;
            while (line[i][0] != '#') {
                char* format = malloc(sizeof(char) * 100);
                for (int i=0; i < 25; i++) {
                    strcat(format, "%x ");
                }
                strcat(format, "\n");

                unsigned char* c = malloc(sizeof(unsigned char) * 25);
                sscanf(line[i], format, &c[0], &c[1], &c[2], &c[3], &c[4], 
                &c[5], &c[6], &c[7], &c[8], &c[9], 
                &c[10], &c[11], &c[12], &c[13], &c[14], 
                &c[15], &c[16], &c[17], &c[18], &c[19], 
                &c[20], &c[21], &c[22], &c[23], &c[24]);

                int toWrite = leftToWrite >= 25 ? 25 : leftToWrite;
                memcpy(&memory[writingPos], c, toWrite);
                writingPos += toWrite;

                leftToWrite -= 25;

                free(format);
                free(c);

                getline(&line[i], &length, inputFiles[i]);

                if ((line[i][0] == '#') && (line[i][1] == '\n')) line[i][1] = '\0';
            }
        }
        
    }
}

void linkSections() {
    for (currentToHead(&rellocationReccords); hasCurrent(&rellocationReccords); next(&rellocationReccords)) {
        Rellocation* r = (Rellocation*)getCurrent(&rellocationReccords);
        /* Dohvatamo sekciju gde treba da upisemo vrednost */
        Section* section = NULL;
        for (int i=0; i < sectionIndex; i++) {
            if (sections[i]->ordinal == r->section && sections[i]->file == r->file) {
                section = sections[i];
                break;
            }
        }
        int pcRel = isPcRel(r);
        /* Vrednost koju upisujemo je start sekcije ciji je ord u r->section*/
        Section* value1 = NULL;
        Symbol* value2 = NULL;
        //if (!pcRel) {
        if (r->symbol <= lastSectionId) { // U pitanju je lokalan simbol
        /* Dohvatamo sekciju ciji start upisujemo */
            for (int i=0; i < sectionIndex; i++) {
                if (sections[i]->ordinalInFile[r->file] == r->symbol && sections[i]->file == r->file) {
                    value1 = sections[i];
                    break;
                }
            }
            short temp = (memory[section->start + r->offset] & 0xFF) | ((memory[section->start + r->offset+1] & 0xFF) << 8);
            temp += value1->start;
            memory[section->start + r->offset] = temp & 0xFF;
            memory[section->start + r->offset + 1] = (temp & 0xFF00) >> 8;;
        } else { // Symbol je globalan
            /* Dhovatamo simbol koji upisujemo */
            //value2 = findByOrdinal(&symbolTable, r->symbol);
            value2 = findByOrdinalInFile(&symbolTable, r->symbol, r->file);
            /*Njemu je dodat pocetak sekcije u updateGlobal 
            * tako da mozemo samo da dodamo njegovu vrednost na odg mesto
            */
            short temp = (memory[section->start + r->offset] & 0xFF) | ((memory[section->start + r->offset+1] & 0xFF) << 8);
            temp += value2->value;
            memory[section->start + r->offset] = temp & 0xFF;
            memory[section->start + r->offset + 1] = (temp & 0xFF00) >> 8;
        }
        if (pcRel) {
            short temp = (memory[section->start + r->offset] & 0xFF) | ((memory[section->start + r->offset+1] & 0xFF) << 8);
            temp -= section->start + r->offset;
            memory[section->start + r->offset] = temp & 0xFF;
            memory[section->start + r->offset + 1] = (temp & 0xFF00) >> 8;
        }
    }
}

void updateGlobalSymbols() {
    for (currentToHead(&symbolTable); hasCurrent(&symbolTable); next(&symbolTable)) {
        Symbol* s = (Symbol*)getCurrent(&symbolTable);
        if (/*s->linking == 'G' &&*/ s->file != -1) {
            for (int i=0; i < sectionIndex; i++) {
                if (sections[i]->ordinal == s->section && sections[i]->file == s->file) {
                    s->value += sections[i]->start;
                }
            }
        }
    }
}

void linkAndLoadSections() {
    int freeMem = (STACK_START - STACK_SIZE) - (IVT_NUM_ENTRIES * IVT_ENTRY_SIZE) + 1;
    if (sumSize() > freeMem) {
        printf("Skecije ne mogu da stanu u memoriju!\nKraj programa\n");
        exit(1);
    }

    /* Dodaj start na sve sekcije za koje je naveden -place argument */
    if (mode == MODE_HEX) {
        for (int i=0; i < placeArgsIndex; i++) {
            for (int j=0; j < sectionIndex; j++) {
                if (strcmp(placeArgs[i]->name, sections[j]->name) == 0) {
                    sections[j]->start += placeArgs[i]->start;
                    sections[j]->placed = 1;
                }
            }        
        }
    }
        

    /* Sve preostale sekcije se smestaju iza poslednje koja je na lastEnd */
    int lastEnd = maxEnd();
    /* Prva validna adresa u memoriji, nakon IVT tabele ako nije naveden nijedan place */
    //if (placeArgsIndex == 0) lastEnd = IVT_NUM_ENTRIES * IVT_ENTRY_SIZE;

    /* Dodaj lastEnd na sve za koje nije naveden -place argument i uvecaj lastEnd*/
    for (int i=0; i < sectionIndex; i++) {
        if (!sections[i]->placed) {
            sections[i]->start += lastEnd;
            for (int j=i+1; j < sectionIndex; j++) {
                if (strcmp(sections[i]->name, sections[j]->name) == 0) {
                    sections[j]->start += lastEnd;
                    sections[j]->placed = 1;
                }
            }
            lastEnd += sections[i]->accuSize;
        }
    }    

    for (int i=0; i < inputFileIndex - 1; i++) {
        for (int j=i+1; j < sectionIndex; j++) {
            int cmpValue1, cmpValue2;
            if (sections[i]->start < sections[j]->start) {
                cmpValue1 = sections[i]->start + sections[i]->size;
                cmpValue2 = sections[j]->start;
            } else {
                cmpValue1 = sections[j]->start + sections[j]->size;
                cmpValue2 = sections[i]->start;
            }
            if (cmpValue1 > cmpValue2 && mode == MODE_HEX) {
                printf("Preklapanje sekcija!\nKraj programa\n");
                exit(1);
            }
        }
    }

    updateGlobalSymbols();

    loadSections();

    linkSections();
}

void link() {
    //Prvo treba se pozove prekidna rutina u nultom ulazus


    // Procitaj i popuni tabelu simbola, iz SVIH ulaznih fajlova
    // Kao i zapise o relokacijama za svaku sekciju
    initialize();

    // Ucitaj sekcije u memoriju i javi gresku ako ne mogu da stanu ili se preklapaju razlictte
    // Globalne podatke smesti zajedno u poseban deo memorije
    //loadSections();

    // Na osnovu relokacionih zapisa i vrednosti simbola i sekcija azuriraj
    linkAndLoadSections();

    if (mode == MODE_LINKABLE) {
        writeLinkable();
    } else {
        writeHex2();
    }
}