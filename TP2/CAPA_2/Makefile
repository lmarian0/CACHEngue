CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -fno-omit-frame-pointer
CAPA_3 := ../CAPA_3

ASM_FLOAT := $(CAPA_3)/float_to_int.o
ASM_PLUS := $(CAPA_3)/gini_plus_one.o

# Genera la libreria dinamica integrando C y ASM [cite: 21, 29]
libgini.so: gini_calc.c $(ASM_FLOAT) $(ASM_PLUS)
	$(CC) -shared -o $@ -fPIC gini_calc.c $(ASM_FLOAT) $(ASM_PLUS)

# Para la demo de GDB pedida por la consigna [cite: 28]
gdb_demo: gdb_demo.c $(ASM_FLOAT) $(ASM_PLUS)
	$(CC) $(CFLAGS) -o $@ gdb_demo.c $(ASM_FLOAT) $(ASM_PLUS)

$(ASM_FLOAT): $(CAPA_3)/float_to_int.asm
	$(CC) $(CFLAGS) -x assembler -c $< -o $@

$(ASM_PLUS): $(CAPA_3)/gini_plus_one.asm
	$(CC) $(CFLAGS) -x assembler -c $< -o $@

clean:
	rm -f $(ASM_FLOAT) $(ASM_PLUS) *.so gdb_demo

.PHONY: libgini.so gdb_demo clean
