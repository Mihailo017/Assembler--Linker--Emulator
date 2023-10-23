#include "assembler.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern FILE* yyin;
extern FILE* out;

int main(int argc, char* argv[]) {

    if (argc < 3) {
      printf("Nedovoljan broj argumenata!\n");
      return 1;
    }  

    if (strcmp(argv[1], "-o") != 0) {
      printf("Primer pozivanja programa: ./asembler -o izlaz.o ulaz.o\n\n");
      exit(1);
    }

    yyin = fopen(argv[3], "r");
    out = fopen(argv[2], "w");
  
    assemble();

    fclose(out);
    return 0;
}
