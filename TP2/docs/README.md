# TP2 - Vision general y depuracion con GDB

## 1) Objetivo del proyecto

Este TP2 implementa un flujo por capas para procesar valores de indice Gini:

- Capa 1 (Python): obtiene datos desde API y llama a la libreria nativa con ctypes.
- Capa 2 (C): expone una funcion puente (`process_gini`) para ser invocada desde Python.
- Capa 3 (ASM x86-64): implementa la logica de conversion y ajuste (`float_to_int`, `gini_plus_one`).

La parte de depuracion se muestra con un programa de C puro (`gdb_demo`) para observar el stack antes, durante y despues de una llamada de funcion.

## 2) Estructura relevante

- `CAPA_1/fetch_gini.py`: consumo de API y llamada a `libgini.so`.
- `CAPA_2/gini_calc.c`: puente C hacia ASM.
- `CAPA_2/gdb_demo.c`: demo de depuracion en C puro.
- `CAPA_2/Makefile`: build de `gdb_demo` y dependencias ASM.
- `CAPA_3/float_to_int.asm` y `CAPA_3/gini_plus_one.asm`: funciones ensamblador.

## 3) Que se busca observar en GDB

La consigna pide mostrar el estado del area de memoria del stack:

- Antes de entrar a la funcion (`stack_demo`).
- Durante la ejecucion de la funcion (con frame activo).
- Despues de retornar al llamador (`main`).

Ademas, se explica que representan variables locales, stack frame y el registro `rbp`.

## 4) Compilacion para depurar

Ejecutar desde `TP2/CAPA_2`:

```bash
make gdb_demo
```

Ese binario se compila con simbolos de debug y sin optimizaciones agresivas, para que GDB muestre mejor variables y stack frame.

## 5) Comandos de GDB (comentados)

Abrir GDB:

```bash
gdb ./gdb_demo
```

Secuencia recomendada:

```gdb
set disassembly-flavor intel
break main
break stack_demo
run
```

Explicacion:

- `set disassembly-flavor intel`: muestra desensamblado en sintaxis Intel.
- `break main`: frena al inicio del programa (estado "antes").
- `break stack_demo`: frena al entrar a la funcion que crea variables locales.
- `run`: ejecuta hasta el primer breakpoint.

### 5.1 Estado del stack antes de entrar a la funcion

```gdb
x/24gx $rsp
info frame
info registers rsp rbp
```

Que hace cada comando:

- `x/24gx $rsp`: examina 24 "giant words" (8 bytes) desde la direccion apuntada por `rsp`.
- `info frame`: muestra detalles del frame actual (direccion de retorno, base del frame, etc.).
- `info registers rsp rbp`: muestra punteros de stack y base pointer.

Que se observa:

- Contexto del frame de `main`.
- Direcciones de retorno y datos del stack del llamador actual.

### 5.2 Estado durante `stack_demo`

```gdb
continue
x/24gx $rsp
info frame
info registers rsp rbp
info locals
print left
print right
print frame_data
```

Que hace cada comando:

- `continue`: reanuda hasta el siguiente breakpoint (`stack_demo`).
- `info locals`: lista variables locales visibles en el frame actual.
- `print ...`: inspecciona valores concretos de locales.

Que se observa:

- Nuevo stack frame activo para `stack_demo`.
- Variables locales (`left`, `right`, `frame_data`) ubicadas en ese frame.
- `rbp` como referencia estable del frame, mientras `rsp` puede moverse por pushes/pops/calls.

### 5.3 Estado despues de retornar

```gdb
finish
x/24gx $rsp
info frame
info registers rsp rbp
```

Que hace:

- `finish`: ejecuta hasta salir de la funcion actual y volver al llamador.

Que se observa:

- El frame de `stack_demo` ya no esta activo.
- Se vuelve al contexto de `main`.
- El area de stack refleja el estado posterior al retorno.

## 6) Conceptos clave

### Variables locales

Son variables declaradas dentro de una funcion. Su vida util existe mientras ese frame esta activo en stack. En el demo:

- `left`
- `right`
- `frame_data`

### Stack frame

Es el bloque de stack asociado a una invocacion de funcion. Suele contener:

- direccion de retorno,
- base del frame previo,
- espacio para variables locales,
- posibles temporales/alineacion.

### Registro `rbp`

En x86-64, `rbp` suele usarse como frame pointer:

- apunta a una posicion fija dentro del frame actual,
- facilita acceder a variables y parametros con offsets constantes,
- ayuda a que el debugging sea mas claro (especialmente con `-fno-omit-frame-pointer`).

## 7) Capturas de consola incluidas

Las siguientes capturas estan en esta carpeta y documentan la sesion de depuracion:

En estas capturas tambien se puede observar que el stack crece hacia direcciones bajas de memoria; por eso, al entrar al frame de la funcion, los offsets de las variables locales respecto de EBP/RBP aparecen en posiciones mas bajas.

- [Captura 1](Captura%20desde%202026-04-20%2013-33-04.png)
- [Captura 2](Captura%20desde%202026-04-20%2013-33-50.png)
- [Captura 3](Captura%20desde%202026-04-20%2013-34-47.png)
- [Captura 4](Captura%20desde%202026-04-20%2013-36-14.png)

## 8) Resumen

Con esta secuencia en GDB se verifica exactamente lo pedido por la consigna: estado del stack antes, durante y despues de la funcion, con inspeccion explicita de variables locales, stack frame y `rbp`.
