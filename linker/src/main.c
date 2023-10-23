#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processor.h"
#include "section.h"
#include "linker.h"

void parsePlace(char* place) {
    while (*place != '=') place++;
    place++;
    char* name = malloc(strlen(place));
    int i;
    for (i=0; place[i] != '@'; i++) name[i] = place[i];
    int start = strtol(place+i+3, NULL, 16);
    placeArgs[placeArgsIndex++] = createSection(name, start);
    placeArgs[placeArgsIndex-1]->placed = 1;
}

int main(int argc, char* argv[]) {

    inputFileIndex = 0;
    placeArgsIndex = 0;
    sectionIndex = 0;
    mode = MODE_INITIAL;
    for (int i=0; i < MAX_INPUT_FILES; i++) {
        inputFiles[i] = NULL;
    }

    for (int j=0; j < MAX_SECTIONS; j++)
        sections[j] = NULL;

    for (int i=0; i<MAX_INPUT_FILES * MAX_SECTIONS; i++) placeArgs[i] = NULL;

    for (int i=1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-linkable") == 0) {
                mode = MODE_LINKABLE;
            } else if (strcmp(argv[i], "-hex") == 0) {
                mode = MODE_HEX;
            } else {
                parsePlace(argv[i]);
            }
        }
        else {
            inputFiles[inputFileIndex++] = fopen(argv[i], "r");
        }
    }

    if (mode == MODE_INITIAL) {
        printf("Mora da se navede bar jedna od -hex i -linkable opcija!");
        exit(1);
    }

    link();

    for (int j=0; j < sectionIndex; j++) free(sections[j]);

    return 0;
}