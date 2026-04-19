# Genera la librería dinámica integrando C y ASM [cite: 21, 29]
libgini.so: gini_calc.c float_to_int.o gini_plus_one.o
	gcc -shared -o libgini.so -fPIC gini_calc.c float_to_int.o gini_plus_one.o

# Para la demo de GDB pedida por la consigna [cite: 28]
gdb_demo: gdb_demo.c float_to_int.o gini_plus_one.o
	gcc -g -o gdb_demo gdb_demo.c float_to_int.o gini_plus_one.o

clean:
	rm -f *.o *.so gdb_demo
