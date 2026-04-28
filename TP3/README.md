# TP3: Transición a Modo Protegido (x86)

Este proyecto implementa el paso de un procesador x86 desde el **Modo Real** (16 bits) al **Modo Protegido** (32 bits), configurando manualmente las estructuras de control de hardware sin utilizar macros.

## 🛠️ Implementación Técnica

### 1. Configuración de la GDT (Global Descriptor Table)
Se definieron manualmente los descriptores de segmento para establecer el control de la memoria:
* **Segmentos Diferenciados**: Se configuraron bases distintas para el segmento de código (`0x00000000`) y el de datos (`0x00020000`), logrando espacios de memoria físicamente separados.
* **Control de Acceso**: Se implementaron descriptores con diferentes privilegios para testear la protección de hardware.

### 2. El Salto a Modo Protegido
El código realiza la secuencia crítica de activación:
1. Desactivación de interrupciones (`cli`).
2. Carga de la tabla mediante la instrucción `lgdt`.
3. Activación del bit PE en el registro `CR0`.
4. Ejecución de un **Far Jump** para limpiar el pipeline y cargar el selector de código en `CS`.

## 🧪 Experimento de Protección de Memoria

Se modificó el bit de acceso del segmento de datos a **Solo Lectura** (`0x90`). 

* **¿Qué sucede al intentar escribir?**: Al ejecutar la instrucción `movl $0xDEADBEEF, (0x0)`, el procesador detecta una violación de permisos.
* **¿Qué debería suceder a continuación?**: El hardware genera una excepción de **Protección General (#GP)**. Dado que no existe una IDT configurada, esto deriva en una **Triple Falta**, provocando el reinicio constante de la máquina virtual (visto como parpadeo en QEMU).

## 📊 Verificación con GDB

* **Valor de registros de segmento**: En modo protegido, los registros como `DS` o `ES` se cargan con el valor **0x10**.
* **¿Por qué?**: Porque en este modo los registros funcionan como **Selectores**. El valor `0x10` es un índice que apunta a la entrada correspondiente en la GDT, donde el procesador consulta la base, el límite y los permisos antes de ejecutar cualquier operación de memoria.
