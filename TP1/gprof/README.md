## **Paso 1: creación de perfiles habilitada durante la compilación**

En este primer paso, debemos asegurarnos de que la generación de perfiles esté habilitada cuando se complete la compilación del código. Esto es posible al agregar la opción '-pg' en el paso de compilación.  
Entonces, compilemos nuestro código con la opción '-pg':

**$ gcc \-Wall \-pg test\_gprof.c test\_gprof\_new.c \-o test\_gprof**

![](image13.png)

## **Paso 2: Ejecutar el código**

En el segundo paso, se ejecuta el archivo binario producido como resultado del paso 1 (arriba) para que se pueda generar la información de perfiles.

**$ ls**  
**$ ./test\_gprof**   
**$ ls**

![](image7.png)

Entonces vemos que cuando se ejecuta el binario, se genera un nuevo archivo 'gmon.out' en el directorio de trabajo actual.

## **Paso 3: Ejecute la herramienta gprof**

En este paso, la herramienta gprof se ejecuta con el nombre del ejecutable y el 'gmon.out' generado anteriormente como argumento. Esto produce un archivo de análisis que contiene toda la información de perfil deseada.

**$  gprof test\_gprof gmon.out \> analysis.txt**  
**$ ls**

![](image12.png)

Entonces vemos que se generó un archivo llamado 'analysis.txt'.

### **Comprensión de la información de perfil**

Cómo se produjo anteriormente, toda la información de perfil ahora está presente en 'analysis.txt'. Echemos un vistazo a este archivo de texto:

![](image4.png)

* **%** el porcentaje del tiempo total de funcionamiento del programa de tiempo utilizado para esta función.  
* **cumulative** una suma corriente del número de segundos contabilizados, segundos por esta función y las enumeradas anteriormente.  
* **self** el número de segundos contabilizados por este los segundos funcionan solos. Este es el tipo principal para este listado.  
* **calls** al número de veces que se invocó esta función, si esta función está perfilada, de lo contrario en blanco.  
* **self** el número promedio de milisegundos gastados en este ms/función de llamada por llamada, si esta función está perfilada, de lo contrario en blanco.  
* **total** el número promedio de milisegundos gastados en este función ms/call y sus descendientes por llamada, si esto la función está perfilada, de lo contrario, en blanco.  
* **name** el nombre de la función. Este es el tipo menor para este listado. El índice muestra la ubicación de la función en la lista de gprof. Si el índice es entre paréntesis muestra dónde aparecería en el listado de gprof si fuera a ser impreso.

![](image17.png)

Esta tabla describe el árbol de llamadas del programa y fue ordenada por la cantidad total de tiempo empleado en cada función y sus hijos.

Cada entrada en esta tabla consta de varias líneas. La línea con el número de índice en el margen izquierdo enumera la función actual. Las líneas arriba enumeran las funciones que llamaron a esta función, y las líneas debajo enumeran las funciones a las que llama.  
Esta línea enumera:

* **index** :un número único dado a cada elemento de la tabla. Los números de índice se ordenan numéricamente. El número de índice está impreso al lado de cada nombre de función para que es más fácil buscar dónde está la función en la tabla.  
* **% time** :este es el porcentaje del tiempo 'total' que se dedicó en esta función y sus hijos. Tenga en cuenta que debido a diferentes puntos de vista, funciones excluidas por opciones, etc. estos números NO sumarán 100%.  
* **self** :esta es la cantidad total de tiempo empleado en esta función.  
* **children :**esta es la cantidad total de tiempo propagado en esta función por sus hijos.  
* **calls** :este es el número de veces que se llamó a la función. Si la función se llama a sí misma recursivamente, el número solo incluye llamadas no recursivas, y es seguido por un \`+' y el número de llamadas recursivas.  
* **name** :el nombre de la función actual. El número de índice es impreso después de él. Si la función es miembro de un ciclo, el número de ciclo se imprime entre el nombre de la función y el número de índice.

Para los padres de la función, los campos tienen los siguientes significados:

* **self** :esta es la cantidad de tiempo que se propagó directamente de la función a este padre.  
* **childs** :esta es la cantidad de tiempo que se propagó desde los hijos de la función en este padre.  
* **calls** :este es el número de veces que este padre llamó al función \`/' el número total de veces que la función fue llamada. Las llamadas recursivas a la función no son incluidas en el número después de \`/'.  
* **name** :este es el nombre del padre. El índice de los padres el número se imprime después. Si el padre es un miembro de un ciclo, el número de ciclo se imprime entre el nombre y el número de índice.

Si no se pueden determinar los padres de la función, la palabra \`' está impreso en el campo \`nombre', y todos los demás campos están en blanco.

Para los hijos de la función, los campos tienen los siguientes significados:

* **self** :esta es la cantidad de tiempo que se propagó directamente del hijo a la función.  
* **children** :esta es la cantidad de tiempo que se propagó desde el los hijos del hijo a la función.  
* **called** :este es el número de veces que la función llamada este hijo \`/' el número total de veces que el hijo fue llamado Las llamadas recursivas del hijo no son aparece en el número después de \`/'.  
* **name** :este es el nombre del hijo. el índice del hijo el número se imprime después. Si el hijo es miembro de un ciclo, el número de ciclo se imprime entre el nombre y el número de índice.

Si hay ciclos (círculos) en el gráfico de llamadas, hay un entrada para el ciclo como un todo. Esta entrada muestra quién llamó al ciclo (como padres) y los miembros del ciclo (como hijos).  
La entrada de llamadas recursivas \`+' muestra el número de llamadas de función que eran internos al ciclo, y la entrada de llamadas para cada miembro muestra, para ese miembro, ¿cuántas veces fue llamado de otros miembros de el ciclo.

![](image3.png)

Como ya se ha comentado, este archivo se divide en dos partes principales:  
1\. Perfil plano

2\. Gráfico de llamadas  
Las columnas individuales (tanto del perfil plano como del gráfico de llamadas) se explican detalladamente en el propio archivo de salida.

### **1\. Suprima la impresión de funciones declaradas estáticamente (privadas) usando \-a**  **$ gprof \-a test\_gprof gmon.out \> analysis.txt** 

![](image2.png)

![](image18.png)  
...  
...  
...  
![](image10.png)  
...  
...  
…

Entonces vemos que no hay información relacionada con func2 (que se define como estática)

### **2\. Elimine los textos detallados usando \-b**

**$ gprof \-b test\_gprof gmon.out \> analysis.txt**

![](image16.png)

![](image9.png)

![](image14.png)

### **3\. Imprima solo perfil plano usando \-p**

**$ gprof \-p \-b test\_gprof gmon.out \> analysis.txt**

![](image15.png)

![](image18.png)

**4\. Imprimir información relacionada con funciones específicas en perfil plano**

**$ gprof \-pfunc1 \-b test\_gprof gmon.out \> analysis.txt**

![](image1.png)

![](image8.png)

## **Genere un gráfico** 

gprof2dot es una herramienta que puede crear una visualización de la salida de gprof. 

instalar gprof2dot:   
**$ pip install gprof2dot** 

instalar graphviz (que es necesario si va a hacer gráficos de "puntos" como el siguiente):   
**$sudo apt install graphviz** 

![](image5.png)

![](image11.png)

## **Conclusión del Análisis de Time Profiling**

Al contrastar el gráfico de llamadas (call graph) generado por **`gprof2dot`** con el código fuente en C, podemos confirmar que la distribución del tiempo de CPU es directamente proporcional a la carga de procesamiento (en este caso, la cantidad de iteraciones) asignada a cada función.

Las observaciones principales son:

* **Relación directa entre iteraciones y tiempo de CPU:** La función **`func1`** es el cuello de botella principal del programa, consumiendo el **58.98%** del tiempo total. Al revisar el código (**`test_gprof.c`**), vemos que esto se debe a que contiene el bucle **`for`** con la mayor cantidad de iteraciones: **`0xffffffff`** (más de 4.290 millones de ciclos). Su "tiempo propio" del **55.48%** refleja exactamente el tiempo que la CPU pasó resolviendo ese bucle específico.  
* **Procesos secundarios:** La función **`func2`** consume un **37.68%** del tiempo de ejecución. Esto tiene un sentido lógico perfecto al ver su código, ya que su bucle itera hasta **`0xafffffff`** (aprox. 2.950 millones de ciclos). Al ser un número menor que el de **`func1`**, el tiempo de CPU requerido disminuye en la misma proporción.  
* **Comprobación de llamadas anidadas:** El gráfico muestra correctamente una llamada desde **`func1`** hacia **`new_func1`** que representa un **3.50%** del tiempo total. El código fuente confirma esta estructura: **`func1`** invoca a **`new_func1()`** inmediatamente después de terminar su propio bucle. Como **`new_func1`** tiene un límite de iteración mucho menor (**`0xfffff66`**, unos 268 millones de ciclos), su impacto en el tiempo global del programa es mínimo, tal como lo refleja el gráfico.

**Resumen:** La herramienta de profiling logró mapear con total precisión el comportamiento del código fuente. Queda demostrado que para optimizar el tiempo de ejecución de este programa, cualquier modificación debería enfocarse primero en reducir la complejidad computacional del bucle principal dentro de **`func1`**.


