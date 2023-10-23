#include "rellocation.h"
#include "linker.h"
#include <stdlib.h>
#include <string.h>

Rellocation* createRellocation() {
    Rellocation* relloc = malloc(sizeof(Rellocation));
    relloc->type = malloc(10*sizeof(char));
    addTail(&rellocationReccords, relloc);
    return relloc;
}

void readRellocationReccords() {
    for (int i=0; i < inputFileIndex; i++) {

        if (!rellocationFound[i]) continue;

        size_t length = LINE_LENGTH;

        while (1) {
            int section = 0;

            /* Header je procitan od strane readSectionTable ili readSymbolTable */
            char* temp = malloc(sizeof(char) * SECTION_NAME_LENGTH);
            sscanf(line[i], "%s", temp);
            
            if (strlen(temp) != 1) {
                char* sectionName =  temp + 6; // Skini #.ret. sa pocetka
                for (int j=0; j<sectionIndex; j++)
                    if (strcmp(sections[j]->name, sectionName) == 0) {
                        section = sections[j]->ordinal;
                        break;
                    }
                if (!section) {
                    printf("Greska! Sekcija iz relok zapisa ne postoji!\n");
                    exit(1);
                }
            } else return;
            free(temp);

            /* Procitaj prvu liniju sa sadrzajem tj relokacionim zapisima
            * Citaj sve do naredne linije koja pocinje sa #, to je sadrzaj prve sekcije
            */
            getline(&line[i], &length, inputFiles[i]);
            while (line[i][0] != '#') {
                
                Rellocation* relloc = createRellocation();
                sscanf(line[i], RELLOC_RECCORD_LINE, &relloc->offset, relloc->type, &relloc->symbol);
                relloc->section = section;
                relloc->file = i;
                getline(&line[i], &length, inputFiles[i]);

            }
            if (line[i][1] == ' ' || strlen(line[i]) == 1) break;
        }
    }   
}

void printRelloc() {
    for (currentToHead(&rellocationReccords); hasCurrent(&rellocationReccords); next(&rellocationReccords)) {
        Rellocation* r = (Rellocation*)getCurrent(&rellocationReccords);
        fprintf(out, "%d\t%d\t%s\t%d\t", r->section, r->file, r->type, r->symbol);
        fprintf(out, "%0*x\n", 4 ,r->offset);
    }
    printf("\n");
}

int isPcRel(Rellocation* r) {
    return strcmp(r->type, R_PCREL) == 0 ? 1 : 0;
}