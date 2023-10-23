#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulator.h"

FILE* in;
FILE* out;

int main(int argc, char* argv[]) {

    in = fopen(argv[1], "r");
    out = fopen("izlaz", "w");

    emulate();

    return 0;
}