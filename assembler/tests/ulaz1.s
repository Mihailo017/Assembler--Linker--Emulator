.section .data

.equ n, 20

a: .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
a_end:
b:

.skip 20

.section .text

    ldr r0, $n
    ldr r1, $b
    ldr r2, $a_end
    ldr r3, $2
    sub r2, r3
    div r5, r3
loop:
    ldr r4, [r2]
    str r4, [r1]
    sub r2, r3
    add r1, r3
    sub r0, r5
    jne %loop
    halt

.end
