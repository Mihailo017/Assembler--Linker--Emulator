.section text
	

	ldr r0, $a # komentar
	ldr r1, b
	ldr r2, %c
	ldr r3, r2 # komentar
	ldr r4, [r3]
	ldr r5, [r4 + b]

	jmp a
	jmp %b
	jmp *b
	jmp *r0
	jmp *[r1]
	jmp *[r2 + c]

.extern a, b, c

.end

