# Laboratorio 3: Compilar y correr una aplicación sin SO (Bare-metal)

## Parte 2: Compilar y ejecutar los ejemplos

### Paso 1: Instalar qemu, compilar y ejecutar los ejemplos

**Ejecución de `bios_hello_world`:**
- Comandos ejecutados: 
  ```bash
  sudo apt install qemu-system-x86
  ./run bios_hello_world
  ```
- **Resultado:** Se ejecutó QEMU exitosamente usando las interrupciones de video del BIOS (modo real a 16 bits). En primer lugar, se corroboró el funcionamiento con el mensaje de prueba original. Posteriormente, tras modificar el código fuente en ensamblador (`bios_hello_world.S`), se volvió a compilar y ejecutar, logrando mostrar exitosamente por pantalla en el emulador nuestro mensaje personalizado.

![Primera ejecucion de codigo](img/bios_hello_world1.png)
![Segunda ejecucion de codigo, cambio de mensaje](img/bios_hello_world2.png)

**Ejecución de `protected_mode`:**
- Comandos ejecutados:
  ```bash
  ./run protected_mode
  ```
- **Resultado:** Al ejecutar este segundo programa, la CPU cambió a modo protegido (32 bits) de manera explícita y se escribió el mensaje original directamente sobre la memoria de video VGA (bypasseando por completo el sistema BIOS). A continuación, tal como se sugirió, modificamos el código fuente (`protected_mode.S`) con el mensaje `"hello world - Protected mode from CACHEngue"`. Volvimos a compilar mediante el script `run` y observamos en el emulador cómo se plasmó en pantalla el cambio esperado.

![Primera ejecución de codigo modo protegido](img/protected_mode1.png)
![Segunda ejecución, cambio de mensaje modo protegido](img/protected_mode2.png)


### Paso 2: Depurar los ejemplos
- Comandos ejecutados para depurar:
  ```bash
  ./run bios_hello_world debug
  ```
- **Observaciones de GDB:** Al ejecutar el modo depuración (y utilizando `gdb-dashboard`), la ejecución de QEMU se pausa y GDB se conecta de forma remota. Automáticamente, GDB inserta un *breakpoint* en la primera línea de ejecución del bootloader de la BIOS, que corresponde a la dirección de memoria `0x00007c00`. 
En la salida del depurador observamos que se detiene sobre la instrucción `BEGIN` del archivo `bios_hello_world.S`. El código máquina desensamblado muestra cómo este macro inicializa el entorno de 16 bits para el arranque, deshabilitando interrupciones (`cli`), haciendo un salto largo (`ljmp`) para asegurar el valor de `CS`, y procediendo a inicializar los demás registros de segmento (`DS`, `ES`, `FS`, `GS`, `SS`) a 0. A partir de acá, podemos utilizar herramientas de GDB como `stepi` o `nexti` para ejecutar el simulador instrucción a instrucción, visualizando en tiempo real cómo cambian los registros del procesador.

---

## Parte 3: Grabar la imagen y correrla en HW real

### Paso 1: Determinar el driver asignado al dispositivo
Para identificar el pendrive utilizamos el comando:
```bash
sudo fdisk -l | grep sd
```
- **Dispositivo identificado:** Tras analizar la salida, determinamos que los discos internos son `/dev/sda` (Windows de ~240GB) y `/dev/sdb` (Linux de ~1TB). Efectivamente el pendrive conectado corresponde al dispositivo **`/dev/sdc`** (con un tamaño de 14.91 GiB, formateado previamente en FAT32).

### Paso 2: Grabar la imagen de disco

- Comando utilizado para grabar en el pendrive:
  ```bash
  sudo dd if=x86-bare-metal-examples/protected_mode.img of=/dev/sdc bs=4M status=progress
  ```
*(Aseguramos colocar `/dev/sdc` como `of`, ya que equivocarse podría sobreescribir el disco principal).*

### Resultado en Hardware Real (Alternativa Virtualizada)
- **Comportamiento al bootear el pendrive físico:** Debido a las configuraciones de hardware moderno (UEFI y Secure Boot) requeridas para correr un entorno Dual Boot con Windows y Linux Mint, alterar la BIOS hacia modo Legacy (MBR) resultaba contraproducente y riesgoso para el sistema anfitrión.
Para sortear este obstáculo y aún así comprobar empíricamente que el dispositivo físico fue flasheado de manera correcta como medio de arranque, optamos por utilizar QEMU para leer y bootear directamente el bloque de hardware del pendrive (y no la imagen `.img` local).

Esto se logró ejecutando el emulador con privilegios para leer nuestra unidad `/dev/sdc`:
```bash
sudo qemu-system-x86_64 -hda /dev/sdc
```

**Resultado:** QEMU leyó exitosamente el Master Boot Record (MBR) que plasmamos en el pendrive real y procedió a arrancar la aplicación bare-metal. Verificamos que el emulador pasó a modo protegido (32 bits) imprimiendo el texto modificado en pantalla de manera instantánea, dejando en evidencia que el dispositivo USB físico es efectivamente booteable.

![booteo desde pendrive](image.png)
