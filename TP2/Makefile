CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
ROOT := $(CURDIR)
CAPA_1 := $(ROOT)/CAPA_1
CAPA_2 := $(ROOT)/CAPA_2
CAPA_3 := $(ROOT)/CAPA_3

LIB_TARGET := $(CAPA_1)/libgini.so
ASM_OBJ := $(CAPA_3)/gini_plus_one.o

all: $(LIB_TARGET)

$(ASM_OBJ): $(CAPA_3)/gini_plus_one.asm
	$(CC) $(CFLAGS) -x assembler -c $< -o $@

$(LIB_TARGET): $(CAPA_2)/gini_calc.c $(ASM_OBJ)
	$(CC) -shared -o $@ -fPIC $(CAPA_2)/gini_calc.c $(ASM_OBJ)

clean:
	rm -f $(ASM_OBJ) $(LIB_TARGET)

.PHONY: all clean
