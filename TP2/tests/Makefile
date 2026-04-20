CC = gcc
PYTHON = python3
CFLAGS = -Wall -Wextra -O2
ROOT := $(abspath ..)
CAPA_1 := $(ROOT)/CAPA_1
CAPA_2 := $(ROOT)/CAPA_2
CAPA_3 := $(ROOT)/CAPA_3

ASM_FLOAT := $(CAPA_3)/float_to_int.o
ASM_PLUS := $(CAPA_3)/gini_plus_one.o
TEST_FLOAT := test_float_to_int
TEST_PLUS := test_gini_plus_one
TEST_PROCESS := test_process_gini

all: build-root test-python test-c test-integration

build-root:
	$(MAKE) -C $(ROOT)

$(ASM_FLOAT): $(CAPA_3)/float_to_int.asm
	$(CC) $(CFLAGS) -x assembler -c $< -o $@

$(ASM_PLUS): $(CAPA_3)/gini_plus_one.asm
	$(CC) $(CFLAGS) -x assembler -c $< -o $@

$(TEST_FLOAT): c/test_float_to_int.c $(ASM_FLOAT)
	$(CC) $(CFLAGS) -o $@ $< $(ASM_FLOAT)

$(TEST_PLUS): c/test_gini_plus_one.c $(ASM_PLUS)
	$(CC) $(CFLAGS) -o $@ $< $(ASM_PLUS)

$(TEST_PROCESS): c/test_process_gini.c $(CAPA_2)/gini_calc.c $(ASM_PLUS)
	$(CC) $(CFLAGS) -o $@ c/test_process_gini.c $(CAPA_2)/gini_calc.c $(ASM_PLUS)

test-python: build-root
	$(PYTHON) -m unittest discover -s python -p 'test_*.py'

test-integration: build-root
	$(PYTHON) -m unittest discover -s python -p 'integration_*.py'

test-c: $(TEST_FLOAT) $(TEST_PLUS) $(TEST_PROCESS)
	./$(TEST_FLOAT)
	./$(TEST_PLUS)
	./$(TEST_PROCESS)

clean:
	rm -f $(ASM_FLOAT) $(ASM_PLUS) $(TEST_FLOAT) $(TEST_PLUS) $(TEST_PROCESS)
	$(MAKE) -C $(ROOT) clean

.PHONY: all build-root test-python test-integration test-c clean
