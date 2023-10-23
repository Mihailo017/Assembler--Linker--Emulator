#include "section.h"
#include "linker.h"
#include "symbol.h"
#include <stdlib.h>
#include <string.h>

Section* createSection(char* name, int start) {
    Section* section = malloc(sizeof(Section));
    section->start = start;
    section->rellocations = NULL;
    section->name = name;
    section->accuSize = 0;
    section->file = 0;
    section->placed = 0;
}

extern int lastSectionId;
void readSectionTable() {
    for (int i=0; i < inputFileIndex; i++) {
        size_t length = LINE_LENGTH;
        /* Procitaj header tabele simbola */
        getline(&line[i], &length, inputFiles[i]);
        if (strcmp(line[i] , SYMTAB_HEADER)) {
            printf("Neprepoznatljiv format ulaznog fajla");
            exit(1);
        }
        /* Citanje linije sa imenima kolona*/
        getline(&line[i], &length, inputFiles[i]);

        /* Citaj sve do naredne linije koja pocinje sa #, to su relokacioni zapisi*/
        getline(&line[i], &length, inputFiles[i]);
        while (line[i][0] != '#') {
            char* name = malloc(sizeof(char) * SECTION_NAME_LENGTH);
            Section* section = createSection(name, 0);
            int temp; char temp1;
            int sectionOrd;
            sscanf(line[i], SYMTAB_LINE, &section->ordinal, &sectionOrd, &temp, &temp1,
            &section->size, section->name);
            section->file = i;
            section->ordinalInFile[i] = section->ordinal;

            if (section->ordinal != sectionOrd) {
                symbolFound[i] = 1;
                free(name); free(section); 
                break;
            }
            
            lastSectionId = section->ordinal;
            /* Skrati sa maks duzine za ime jer vrv nema potrebe da je toliko*/
            char* newName = malloc(strlen(section->name));
            /* Plus jedan da se ne uzme tacka posto ih place stavlja bez tacke*/
            int k = section->name[0] == '.' ? 1 : 0;
            strcpy(newName, name+k);
            section->name = newName;

            //for (int j=0; j < placeArgsIndex; j++)
            /* Ukoliko je sekcija navedena sa -place onda prepisi start i obrisi tu
            *  sekciju
            */
               /* if (strcmp(placeArgs[j]->name, section->name) == 0) {
                    section->start = placeArgs[j]->start;
                }*/

            int firstTime = 0, found = 0;
            for (int j=0; j < sectionIndex; j++) {
                if (strcmp(sections[j]->name, section->name) == 0) {
                    if (!firstTime) {
                        sections[j]->accuSize += section->size;
                    }
                    section->start = sections[j]->start + sections[j]->size;
                    found = 1;
                }
            }
            if (!found) section->accuSize += section->size;
            sections[sectionIndex++] = section;
            getline(&line[i], &length, inputFiles[i]);
        }
        if (!symbolFound[i]) rellocationFound[i] = 1;
    }
}

void printSectionTable() {
    //for (int i=0; i < inputFileIndex; i++)
    fprintf(out, "ord\tfile\tstart\tsize\taccuSize\tname\n");
        for (int j=0; j<sectionIndex; j++) {
            /*printf("%d\t%d\t%d\t%d\t%d\t%s\n", sections[j]->ordinal, sections[j]->file, sections[j]->start,
            sections[j]->size, sections[j]->accuSize, sections[j]->name);*/
            fprintf(out, "%d\t%d\t", sections[j]->ordinal, sections[j]->file);
            fprintf(out, "%0*x\t", 4, sections[j]->start);
            fprintf(out, "%0*x\t", 4, sections[j]->size);
            fprintf(out, "%0*x\t", 4, sections[j]->accuSize);
            fprintf(out, "%s\n", sections[j]->name);
        }
    fprintf(out, "\n");
}


int sumSize() {
    int sum = 0;
    //for (int i=0; i < inputFileIndex; i++)
        for (int j=0; j<sectionIndex; j++)
            sum += sections[j]->size;
    return sum;
}

int maxEnd() {
    int max = 0;
    for (int j=0; j<sectionIndex; j++)
        if (sections[j]->start + sections[j]->size > max && sections[j]->placed) max = sections[j]->start + sections[j]->size;
    return max;
}