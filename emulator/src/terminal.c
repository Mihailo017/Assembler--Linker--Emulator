#include "terminal.h"
#include "processor.h"
#include <stdio.h>
#include <termios.h>

void* inTerm(void* p) {

    while (1) {
        char input;
        scanf("%c", &input);
        if (input != '\n')
            memory[(unsigned short)(0xFF02)] = (unsigned char)input;
        interrupts[TERMINAL] = 1;
    }
}