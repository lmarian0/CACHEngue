# CAPA 3 - Ensamblador (x86-64, System V ABI)

Funciones implementadas en ensamblador para Linux x86-64:

- `long float_to_int(double value)`
  - Recibe `value` en `XMM0`
  - Convierte con truncado usando `cvttsd2si`
  - Retorna entero en `RAX`

- `long gini_plus_one(double value)`
  - Recibe `value` en `XMM0`
  - Convierte con truncado usando `cvttsd2si`
  - Incrementa el resultado con `inc rax`
  - Retorna entero en `RAX`

## Stack y prologo/epilogo

Cada funcion usa prologo/epilogo explicito:

```asm
push rbp
mov rbp, rsp
... cuerpo ...
pop rbp
ret
```

Esto respeta el manejo de stack pedido para la capa.

