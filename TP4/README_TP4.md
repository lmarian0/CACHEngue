# Trabajo Práctico Nº 4: Módulos de Kernel y Seguridad
**Sistemas de Computación - Universidad Nacional de Córdoba**
**Grupo: CACHEngue**
**Integrante:** Lucas (`LucasCerri25`)

Este archivo contiene el desarrollo, las respuestas al cuestionario teórico y los desafíos de la **Parte 1** correspondientes al TP4, enfocado en módulos del kernel de Linux, gestión de memoria, llamadas al sistema y la seguridad del proceso de arranque con **Secure Boot**.

---

##  Preparación y Compilación del Módulo

De acuerdo con las guías del TP, preparamos el entorno e instalamos las herramientas esenciales:
`sudo apt-get update`
`sudo apt-get install build-essential checkinstall kernel-package linux-source hwinfo strace`

### Compilación y Ejecución (Evidencia Local)
Como se observa en las capturas adjuntas, el ciclo de compilación, carga y descarga en nuestra máquina virtual arrojó los siguientes resultados:

1. **Compilación (`make`):** El módulo se compila correctamente utilizando las cabeceras del kernel instalado. Se genera el archivo binario `mimodulo.ko`.
2. **Carga (`sudo insmod mimodulo.ko`):** Se inserta el módulo en el espacio del kernel de forma exitosa.
3. **Verificación (`lsmod | grep mod`):** Comprobamos que el módulo está activo en memoria.
4. **Descarga (`sudo rmmod mimodulo`):** Se remueve de la memoria del sistema de forma limpia.

---

##  Resolución de Desafíos y Cuestionario

### Desafío #1: Empaquetado y Seguridad
* **¿Qué es `checkinstall` y para qué sirve?**
  `checkinstall` crea un paquete binario nativo (`.deb` para Ubuntu) en lugar de copiar archivos sueltos como hace `make install`. Esto permite desinstalar el software limpiamente usando `sudo apt remove`.
* **Evitar módulos no firmados (Rootkits):**
  Los *rootkits* son malware peligroso que corren con máximos privilegios (Ring 0). Para evitar que se carguen en el kernel, se usa la **verificación estricta de firmas digitales**. El kernel rechazará cualquier archivo `.ko` que no esté firmado criptográficamente.

### Desafío #2: Espacios de Memoria y Dispositivos
* **Programa vs. Módulo:**
  Un programa corre en **Espacio de Usuario** aislado y usa la biblioteca `libc` (ej. `printf()`). Un módulo corre en **Espacio de Kernel** con acceso directo al hardware y usa funciones internas como `printk()`.
* **Directorio `/dev`:**
  Contiene archivos especiales que son interfaces hacia los *drivers*. Se dividen en dispositivos de caracteres (transmiten byte a byte, ej. `/dev/tty`) y dispositivos de bloque (transmiten en bloques, ej. `/dev/sda`).

---

### Preguntas del Cuestionario

**1. ¿Qué diferencias se pueden observar entre los dos modinfo?**
El módulo oficial de `bluetooth` está firmado criptográficamente (`sig_id`, `signer`), comprimido (`.ko.zst`) y es *in-tree* (oficial). Nuestro módulo `mimodulo.ko` no está firmado, no está comprimido y es *out-of-tree* (externo).

**2. ¿Qué drivers/módulos están cargados en sus propias PC?**
Se generó el archivo `mis_modulos_lucas.txt` (ya subido al repositorio) para comparar con el grupo.

**3. ¿Cuáles no están cargados pero están disponibles?**
Están en `/lib/modules/`. Si el driver de un dispositivo no está disponible, el sistema reconoce que el hardware está conectado físicamente, pero no sabe cómo comunicarse con él, dejándolo inutilizable.

**4. Información de Hardware Empleado (`hwinfo`)**
Se generó ejecutando `sudo hwinfo --short > info_hardware.txt`.

**5. Diferencia entre un módulo y un programa**
Un programa tiene un `main()` y corre en espacio de usuario. Un módulo responde a eventos (`module_init`, `module_exit`) y corre en espacio de kernel con privilegios absolutos.

**6. Llamadas al sistema de un helloworld en C**
Se usa el comando `strace ./helloworld` para interceptar llamadas como `execve()`, `mmap()` y `write()`.

**7. Segmentation fault**
Ocurre cuando un proceso intenta acceder a memoria sin permisos. El kernel le envía una señal `SIGSEGV` y el programa muere. Si esto mismo pasa dentro del kernel (en un módulo), causa un **Kernel Panic** y congela toda la PC.

**8 y 10. Firmado y Secure Boot**
Se firman usando `sign-file` y un par de claves. Si le paso mi módulo firmado a un compañero con Secure Boot estricto, su PC lo rechazará (`Operation not permitted`) porque no tiene mi clave pública. Deberá importarla primero con `mokutil`.

**11. Parche de Microsoft y Secure Boot**
El parche bloqueó por error versiones válidas de GRUB en sistemas *Dual-Boot*, impidiendo arrancar Linux. Desactivar Secure Boot lo soluciona, pero deja a la PC vulnerable a malware en el arranque. El propósito de Secure Boot es asegurar que solo se ejecute software firmado y confiable al encender la PC.
