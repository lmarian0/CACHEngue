# Demo de GDB

Este directorio incluye un programa de C puro para mostrar el estado del stack con GDB antes, durante y despues de entrar a una funcion.

## Compilacion

Desde `TP2/CAPA_2`:

```bash
make gdb_demo
```

## Ejecucion con GDB

```bash
gdb ./gdb_demo
break main
break stack_demo
run
x/24gx $rsp
continue
x/24gx $rsp
finish
x/24gx $rsp
```

La idea es capturar:

- Antes de llamar a `stack_demo`, desde `main`.
- Durante `stack_demo`, con el stack frame activo.
- Despues de volver a `main` con `finish`.