PROGRAM = asembler

SOURCE_C_LIST = \
./src/main.c \
./src/list.c \
./src/lex.yy.c \
./src/instruction.c \
./src/assembler.c \
./src/symtab.c \
./src/rellocation.c \
./src/section.c \
./src/allocation.c

FLAGS_INCLUDE = \
-Iinc

FLAGS_GCC =
FLAGS_GCC += -Xlinker -lfl

all : $(PROGRAM)

$(PROGRAM) : $(SOURCE_C_LIST) makefile
	gcc -g $(FLAGS_INCLUDE) -o $(@) $(SOURCE_C_LIST) -Xlinker -lfl

./lex.yy.c : input
	flex -o ./src/lex.yy.c input

clean : 
	rm $(PROGRAM)