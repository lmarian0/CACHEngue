# Parte 1: Benchmarks

## ¿Qué es un benchmark?

Un benchmark es un programa o conjunto de programas diseñado para medir el rendimiento de un sistema bajo condiciones controladas y reproducibles. A diferencia de medir el tiempo de ejecución de un programa cualquiera, los benchmarks están construidos para estresar componentes específicos del hardware (CPU, memoria, disco, GPU) y permitir comparaciones entre distintos sistemas.

Existen distintos tipos de benchmarks:

- **Sintéticos**: programas pequeños diseñados específicamente para cargar un componente. Son fáciles de reproducir pero pueden no reflejar el uso real.
- **De núcleo (kernel benchmarks)**: extraen las partes computacionalmente intensivas de aplicaciones reales.
- **Programas reales**: ejecutan aplicaciones completas como compiladores, motores de renderizado o simuladores. Son los más representativos pero también los más difíciles de estandarizar.

---

## Lista de benchmarks seleccionados

A continuación se presenta una tabla con los benchmarks considerados más relevantes para el perfil de uso personal, que incluye compilación de código, gaming y uso general del sistema.

| Tarea diaria | Benchmark |
|---|---|
| Compilar proyectos (C, Python, etc.) | [Build Linux Kernel](https://openbenchmarking.org/test/pts/build-linux-kernel) |
| Jugar videojuegos | Cinebench R23 / Unigine Heaven | 
| Navegar por la web, múltiples pestañas | [7-Zip Compression](https://openbenchmarking.org/test/pts/compress-7zip) | 
| Ejecutar pruebas automatizadas / CI | [Build GCC](https://openbenchmarking.org/test/pts/build-gcc) | 
| Uso general del sistema operativo | [OpenSSL](https://openbenchmarking.org/test/pts/openssl) | 

---

## ¿Cuáles serían más útiles?

**Build Linux Kernel** es el más útil para el perfil de desarrollador. Compilar el kernel de Linux es una tarea real que involucra miles de archivos fuente, requiere un compilador optimizado y estresa simultáneamente la CPU, la memoria y el sistema de almacenamiento.

**7-Zip Compression** es igualmente representativo para el uso general del sistema. La compresión y descompresión de archivos es una tarea frecuente que combina acceso a memoria, cómputo intensivo y operaciones de E/S.

**Para gaming** en este caso, herramientas externas como 
**Cinebench R23** (que mide rendimiento de renderizado 3D, representativo de la carga de CPU en juegos) o **3DMark** (que evalúa la GPU con cargas similares a las de un motor de videojuego moderno) son más apropiadas.

---

## Rendimiento para compilar el kernel de Linux

A continuación se analiza el rendimiento de los procesadores indicados en la consigna utilizando los resultados públicos disponibles en [openbenchmarking.org/test/pts/build-linux-kernel-1.15.0](https://openbenchmarking.org/test/pts/build-linux-kernel-1.15.0).

Los procesadores evaluados son:

- **Intel Core i5-13600K** (referencia base)
- **AMD Ryzen 9 5900X 12-Core**
- **AMD Ryzen 9 7950X 16-Core** (sistema mejorado)

| Procesador | Tiempo promedio (s)|
|---|---|
|Intel Core i5-13600K | 846 +/- 61 |
|AMD Ryzen 9 5900X 12-Core | 901 +/- 77 |
|AMD Ryzen 9 7950X 16-Core | 512 +/- 33 |

### Cálculo de Speedup

El speedup mide la ganancia de rendimiento del sistema mejorado respecto al original:

$$Speedup = \frac{EX_{CPU\,Original}}{EX_{CPU\,Mejorado}} = \frac{T_{i5\text{-}13600K}}{T_{7950X}} = 1.65$$


### Cálculo de Eficiencia

La eficiencia mide qué tan bien se aprovechan los recursos adicionales (en este caso, núcleos):

$$Eficiencia = \frac{Speedup_n}{n} = 2.42$$

donde $n$ es la cantidad de núcleos del procesador mejorado respecto al de referencia.

---

## Conclusiones (solo esta parte 1 por ahora)

Un benchmark es útil en la medida en que representa fielmente las tareas que el usuario realiza. No existe un benchmark universal: la elección correcta depende del perfil de uso. Para un desarrollador que compila código frecuentemente, **Build Linux Kernel** es la métrica más honesta. Para un usuario orientado al gaming, herramientas especializadas como Cinebench o 3DMark son más adecuadas que las disponibles en openbenchmarking.org.

También es importante reconocer que benchmarks como MIPS o FLOPS, si bien son medidas conocidas, no reflejan el rendimiento real de un sistema en tareas cotidianas: el rendimiento real depende de la jerarquía de memoria, el sistema operativo, el compilador y la naturaleza específica de la carga de trabajo.