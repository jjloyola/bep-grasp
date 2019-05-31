# Implementación del procedimiento Greedy Randomized Adaptive Search Procedure (GRASP) para la resolución del Bus Evacuation Problem
### *Autora: Javiera Loyola Vitali*
[Read English Version](./README.md)

### 1. Compilación

Para compilar los archivos fuentes, abrir una consola en el directorio del código y ejecutar el comando `make` o `make all`. Para eliminar los archivos de compilación *.o ejecutar `make clean`.


### 2. Ejecutar el algoritmo

Para obtener un detalle de los parámetros que se deben ingresar, use `./bep-grasp -h`

Para ejecutar el algoritmo, use el siguiente formato 

```bash
./bep-grasp -i {directory}/{instanceName} -s {seed} -ref {lrcSizeProportion} -ithc {maxIterationsHC} -t {timeLimit} -o {outputFilename}
```

Por ejemplo, `./bep-grasp -i instances/real/BEP-5-52-12-50.txt -s 5 -ref 0.3 -ithc 10 -t 10 -o result.txt`


### 3. Estructura de archivos

**main:** maneja los parámetros y llama al resto de funciones para ejecutar el algoritmo

**dataStructures:** contiene todas las definiciones de los objetos principales (Bus, Depot, Shelter y Assembly Point) 

**fileHandler:** permite leer el archivo de entrada, y escribir los resultados en archivos de salida

**problemInstance:** contiene el mapeo de todos los elementos del problema. Los objetos Bus, Depot, etc. existen en memoria, pero el acceso a ellos (las listas con punteros a sus ubicaciones) está en un objeto de tipo ProblemInstance, y en conjunto estos elementos conforman el escenario a resolver.

**solution:** contiene una lista de buses, y cada uno contiene una lista de viajes que compone su ruta de evacuación. Es una especie de duplicado de problemInstance, pero sólo la lista de buses y la lista de viajes (lo que completa una solución). Sirve para ir guardando la mejor solución, o soluciones entre iteraciones, ya que problemInstance se va reseteando cada vez.

**tripMapping:** contiene los objetos que permiten definir un viaje de un nodo a otro.

**solver:** contiene la función con la estructura de GRASP, las funciones necesarias para la construcción de la solución inicial, y las funciones asociadas al algoritmo de búsqueda local Hill Climbing de la fase de postprocesamiento


### 4. Archivos para ejecutar experimentos

Para la ejecución de los experimentos se generaron tres archivos, que pueden ser ejecutados como ./{nombreArchivoBash}.

**bash-bep-Random.sh:** pruebas para las instancias generadas aleatoriamente
**bash-bep-Real.sh:** pruebas de parámetros para las instancias del indendio de valparaíso
**bash-bep-Real_bus-var.sh:** pruebas sobre instancias del incendio, variando el número de buses
