// ARCHIVO: gini_calc.c [cite: 22]
#include <stdio.h>

// Declaramos lo que hará la Persona 3 en Assembler [cite: 23, 24, 25]
extern int float_to_int(double valor);
extern int gini_plus_one(double valor);

// Función que llamará Python [cite: 26]
int process_gini(double value) {
    // Invocamos la lógica de la Capa 3 [cite: 27]
    return gini_plus_one(value);
}
