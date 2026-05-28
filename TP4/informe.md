# Trabajo Práctico Nº 4: Módulos de Kernel y Seguridad

**Sistemas de Computación - Universidad Nacional de Córdoba**  
**Grupo:** CACHEngue  
**Integrantes:** Cerri Lucas, Callovi Lautaro, Rivera Mariano

---

## 1. Introducción

Un **módulo de kernel** es un fragmento de código que puede ser cargado y descargado dinámicamente en el kernel de Linux sin necesidad de reiniciar el sistema. Esta característica permite extender las funcionalidades del kernel en tiempo de ejecución, siendo la base del funcionamiento de drivers de dispositivos, sistemas de archivos y protocolos de red, entre otros.

A diferencia de un sistema monolítico puro, Linux permite esta modularidad manteniendo un núcleo compacto y estable, mientras que las funcionalidades adicionales se agregan según se necesiten.

---

## 2. Preparación del entorno y Compilación del Módulo

### 2.1 Instalación de dependencias

Para poder compilar y gestionar módulos de kernel, se instalaron las herramientas necesarias mediante los siguientes comandos:

```bash
sudo apt-get update
sudo apt-get install build-essential checkinstall kernel-package linux-source hwinfo strace
```

Durante la instalación es normal obtener un error para el paquete `kernel-package` (indicando que `no installation candidate`), ya que fue deprecado y eliminado de los repositorios modernos de Debian/Ubuntu. Los demás paquetes se instalan correctamente. 

El paquete verdaderamente crítico para el desarrollo de módulos es `linux-headers`, que contiene los headers del kernel actualmente en ejecución:

```bash
sudo apt-get install linux-headers-$(uname -r)
```

### 2.2 Compilación del módulo

Desde el directorio del código fuente, se ejecutó el proceso de compilación:

```bash
make
```

El comando `make` utiliza el `Makefile` del proyecto para compilar el código fuente del módulo (`.c`) empleando las cabeceras del kernel instalado y generar el archivo binario del módulo (`.ko` — *kernel object*). Este archivo es el que puede ser cargado dinámicamente en el kernel.

---

## 3. Carga y Descarga del módulo en el Kernel

### 3.1 Inserción con `insmod`

Una vez compilado, el módulo se insertó en el espacio del kernel con:

```bash
sudo insmod mimodulo.ko
```

El comando `insmod` (*insert module*) carga el archivo directamente en la memoria del kernel. Requiere privilegios de superusuario ya que modifica el código que se ejecuta en Ring 0.

### 3.2 Verificación con `dmesg` y `lsmod`

Para confirmar que el módulo se cargó correctamente y observar sus mensajes de inicialización, se consultaron tanto el buffer de mensajes del kernel como los módulos cargados:

```bash
sudo dmesg
lsmod | grep mod
```

`dmesg` muestra el *ring buffer* del kernel, registrando el log (como la función `init_module()`). Por su parte, `lsmod` lee el contenido de `/proc/modules` y comprueba que nuestro módulo esté listado consumiendo memoria y activo.

![Salida dmesg](img/salida_dmesg.png)

### 3.3 Remoción con `rmmod`

Para descargar el módulo de la memoria del sistema de forma limpia se ejecutó:

```bash
sudo rmmod mimodulo
```

El comando `rmmod` elimina el módulo de la memoria, ejecutando su función de salida (`modulo_lin_clean(void)`). Se indica solo el nombre del módulo, sin el `.ko`.

La descarga fue confirmada volviendo a ejecutar `dmesg`, `lsmod` y `cat /proc/modules`; lo que verificó que se eliminó correctamente el objeto de la memoria RAM (aunque el archivo persistía en disco).

![Descarga módulo](img/descarga_modulo.png)

---

## 4. Ciclo de vida de un módulo

El ciclo completo (Compilación -> Carga -> Descarga) de un módulo es el siguiente:

```text
  Compilación          Carga                 Descarga
      │                  │                      │
    make   ──────►   insmod    ──────────►   rmmod
      │                  │                      │
 mimodulo.ko       Vive en RAM            Eliminado de RAM
  (en disco)    (activo en kernel)     (archivo .ko persiste)
```

---

## 5. Resolución de Desafíos 

### Desafío #1: Empaquetado y Seguridad
* **¿Qué es `checkinstall` y para qué sirve?**
  `checkinstall` crea un paquete binario nativo (por ejemplo `.deb` para Ubuntu) interceptando la fase de instalación. Esto es mucho más limpio que copiar archivos sueltos (como hace `make install`), dado que permite gestionar y desinstalar el software formalmente del sistema usando `sudo apt remove`.
* **Evitar módulos no firmados (Rootkits):**
  Los *rootkits* son software malicioso que corren con los máximos privilegios del sistema (Ring 0 del Kernel). Para evitar que modifiquen el kernel cargando código dañino, se utiliza la **verificación estricta de firmas digitales**. Bajo configuraciones como Secure Boot, el kernel rechazará (mediante un error como "Operation not permitted") cargar un archivo `.ko` que no esté correctamente firmado criptográficamente por una entidad de confianza.

### Desafío #2: Espacios de Memoria y Dispositivos
* **Programa vs. Módulo:**
  Un programa convencional corre completamente en el **Espacio de Usuario** (User Space), un entorno aislado y restringido, sirviéndose de bibliotecas como la `libc` (usando por ejemplo `printf()`). Un módulo, en cambio, corre dentro del **Espacio de Kernel** (Kernel Space), con acceso ilimitado a nivel del hardware y gestiona internamente comandos diferentes, utilizando bibliotecas internas como `printk()` en su lugar.
* **Directorio `/dev`:**
  Agrupa archivos especiales denominados "archivos de dispositivos". Éstos sirven como interfaz entre el usuario y los *drivers* debajo del capó. Existen de dos grandes tipos: interfaces de caracteres (Character, transmiten datos byte a byte como `/dev/tty`) y los de bloque (Block, transmiten información empaquetada en bloques fijos, ideal para discos y almacenamiento como `/dev/sda`).

---

## 6. Cuestionario Teórico

**1. ¿Qué diferencias se pueden observar entre los dos `modinfo` (oficial vs el propio)?**
El módulo oficial (por ejemplo el de `bluetooth`) está firmado criptográficamente, contando con campos como `sig_id` y `signer`. A su vez, se encuentra comprimido y empaquetado (como `.ko.zst`) y es *in-tree* (distribuido oficialmente y mantenido dentro del código base del Kernel). Nuestro módulo (`mimodulo.ko`) no tiene firma digital, carece de compresión y es definido como *out-of-tree* (compilado por fuera del ecosistema del código fuente base y con nuestra propia versión extraíble).

**2. ¿Qué drivers/módulos están cargados en sus propias PC?**
Al analizar los outputs proporcionados por el comando `lsmod` de Lucas (`modulos_lucas.txt`) y Mariano (`modulos_mariano.txt`), se pueden realizar apreciaciones claras acerca de cómo y sobre qué hardware se ejecuta cada SO:
* **Entorno de Lucas:** Está corriendo el sistema mediante virtualización. Esto se determina porque posee cargado el driver `vboxguest` (lo que afirma que está operando en una máquina virtual alojada en VirtualBox) y utiliza mayoritariamente drivers virtuales y genéricos que le empaqueta el *hypervisor* (tales como `e1000` para su tarjeta de red o `snd_intel8x0` para la tarjeta de audio emulada).
* **Entorno de Mariano:** Está corriendo Linux de forma nativa directo sobre el hardware, una laptop Lenovo Ideapad (`ideapad_laptop`) procesador de Intel. Cuenta con dispositivos de red reales (`iwlwifi` y `mac80211`), controladores oficiales de Bluetooth cargados y gráficos integrados (`i915`). Adicionalmente, cuenta con el módulo `vboxdrv` junto a módulos de virtualización real de Intel (`kvm` y `kvm_intel`), es decir, es el *Host* que ejecuta y brinda el entorno para las máquinas virtuales.

**3. ¿Cuáles no están cargados pero están disponibles?**
Los miles de módulos disponibles pero no cargados se encuentran almacenados físicamente en la carpeta del sistema `/lib/modules/$(uname -r)/kernel/`. Contienen en su interior todo el abanico pre-compilado para la versión específica del núcleo. Cuando se conecta nuevo hardware, subsistemas del espacio de usuario (como `udev`) detectan la interrupción o evento y se le informa al gestor del kernel para que vaya y cargue dinámicamente mediante `modprobe` ese driver. Si un equipo se conecta físicamente y el driver no está disponible, el sistema lo ve y lo detalla, pero no sabe cómo interpretarlo o entablar comunicación, de modo que queda inutilizable.

**4. Información de Hardware Empleado (`hwinfo`)**
Se generó el registro de todos los componentes de la PC ejecutando `sudo hwinfo --short > info_hardware.txt`.

**5. Diferencia entre un módulo y un programa**
Un programa tiene rutinariamente un punto de inicio `main()` y una ejecución lineal, confinado al espacio de usuario y sus limitaciones. Un módulo no se ejecuta así, sino que lo hace en respuesta al ser llamado ante eventos como la carga y destrucción de sí mismo en el subsistema (`module_init`, `module_exit`), y corre en el espacio de kernel con niveles de privilegios absolutos para accionar y modificar el núcleo.

**6. Llamadas al sistema de un "Hello World" en C**
Se puede ver el hilo real de operaciones de un programa a bajo nivel corriendo `strace ./helloworld`. Permite auditar qué y con qué parámetros se le exige al núcleo interactuar. Se encontrarán syscalls como `execve()`, o gestiones de memoria del microprocesador como `mmap()`, e incluso su impresión simple a pantalla traduciéndose a `write()` hacia stdout.

**7. Segmentation fault (Segfault)**
Un "Segmentation Fault" ocurre primariamente cuando un proceso o hilo de software en el espacio de usuario intenta exceder sus dominios de memoria, acceder a direcciones a las cuales no tiene acceso o que no están correctamente paginadas para usar. Como herramienta defensiva y resolutoria, el Kernel interrumpe con el envío de la señal `SIGSEGV` al proceso ofensor, erradicándolo y matándolo. Si la transgresión y el intento ilícito de memoria ocurre por culpa de un módulo dentro del propio entorno del Kernel, al estar en Anillo 0, decanta irremediablemente en un fallo catastrófico e insalvable en el propio sistema central, desembocando en un **Kernel Panic**, la muerte de procesos y el congelamiento general del equipo, que requiere únicamente un apagado abrupto para resolverse.

**8 y 10. Firmado y Secure Boot (UEFI)**
Se firman (garantizan que los archivos poseen procedencia de autores genuinos y con claves privadas resguardadas) empleando el script `sign-file` pasándole el par de claves asociadas del kernel actual. Si decido pasarme este mismo módulo compilado con otro colega el cual activa su control estricto de Secure Boot sobre placa, al tratar de ejecutar el `insmod` el Kernel rechazará activamente el trabajo (`Operation not permitted`); dado que detectará que esa llave de cifrado no se encuentra en su almacén de llaves. La solución es, previamente, insertar y confíar en esa clave pública del desarrollador utilizando el framework unificado de la herramienta `mokutil`.

**11. Parche de Microsoft y Linux en Secure Boot**
Una actualización de seguridad lanzada por Microsoft bloqueó por defecto -y de modo indiscriminado- firmas previas y legítimas correspondientes a cargadores del gestor GRUB en sistemas con particiones híbridas (*Dual-Boot*), impidiendo de plano acceder al menú o iniciar la distribución de Linux contigua. Un modo de sortear el problema de facto fue entrar al firmware y deshabilitar temporalmente la variable global Secure Boot hasta parchear manualmente o lanzar nuevos arreglos validando de vuelta los GRUB. El inconveniente, empero, estriba en que el sistema pasaba a estar temporalmente vulerable contra los tipos de amenazas de nivel fundamental (*Bootkits*) y malwares subrepticios que se cargan antes del propio Windows o Kernel operativo central. El propósito central por diseño de que tu BIOS contenga el Secure Boot pre-instalado es garantizar que los únicos binarios del ecosistema base que puedan siquiera tomar dominio del arranque del ordenador sean de proveedores y programadores con sus firmas aprobadas, avalados e intactos, combatiendo toda mutación ilícita.