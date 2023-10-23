PROGRAM = linker

SOURCE_C_LIST = \
./src/main.c \
./src/list.c \
./src/section.c \
./src/linker.c \
./src/symbol.c \
./src/rellocation.c \

FLAGS_INCLUDE = \
-Iinc

FLAGS_GCC =
FLAGS_GCC += -Xlinker -lfl

all : $(PROGRAM)

$(PROGRAM) : $(SOURCE_C_LIST) makefile | $(BUILD_DIR)
	gcc -g $(FLAGS_INCLUDE) -o $(@) $(SOURCE_C_LIST) -pthread

clean : 
	rm $(PROGRAM)