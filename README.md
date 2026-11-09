# Acelerador de Funciones: Contador de Bits

Proyecto del Módulo 3 de Lenguajes de Interfaz (SCC-1014).

Autor: [TU NOMBRE]

## De qué trata

El programa compara el rendimiento de dos versiones de la misma función, que cuenta cuántos bits en 1 tiene un número de 64 bits (population count):

- **Versión en C puro:** revisa el número bit por bit. Toma el bit menos significativo, lo suma a un contador, desplaza el número una posición a la derecha y repite hasta que el número llega a cero.
- **Versión en ensamblador inline (x86-64):** usa la instrucción `POPCNT`, que cuenta los bits en 1 directamente en hardware, sin bucle.

El programa genera 100 millones de números aleatorios de 64 bits, ejecuta ambas funciones sobre todos ellos, mide el tiempo de cada una con `clock()`, verifica que los resultados coincidan y calcula cuántas veces es más rápida la versión en ensamblador.

## Archivos

- `contador_bits.c`: código fuente completo (función en C, función en ensamblador y programa principal).
- `README.md`: este archivo.

## Cómo compilar y ejecutar

Se necesita GCC (por ejemplo, MinGW en Windows).

Sin optimizaciones en el C (comparación justa):

```
gcc -O0 -o contador contador_bits.c
./contador
```

Con optimizaciones del compilador (-O2):

```
gcc -O2 -o contador_o2 contador_bits.c
./contador_o2
```

En PowerShell se ejecuta con `.\contador.exe` y `.\contador_o2.exe`.

## Resultados

Los programas imprimen los totales de ambas versiones, un mensaje de verificación y los tiempos. En las pruebas realizadas, los resultados de C y ensamblador coincidieron y la versión en ensamblador fue considerablemente más rápida:

| Compilación | Tiempo en C | Tiempo en ASM | Mejora |
|---|---|---|---|
| -O0 | 9.63 s | 0.33 s | 29.53x |
| -O2 | 4.62 s | 0.28 s | 16.37x |

Los tiempos varían de una ejecución a otra según la computadora y los programas abiertos.

## Por qué es más rápido

La versión en C necesita hasta 64 iteraciones por número y, sin optimizaciones, guarda y lee variables de memoria en cada vuelta. La instrucción `POPCNT` resuelve el conteo completo en una sola instrucción del procesador. Incluso con `-O2`, el compilador no reemplaza el bucle por `POPCNT`, por lo que la versión en ensamblador sigue siendo mucho más rápida.

## Video

Explicación del proyecto: [PEGAR AQUÍ EL ENLACE DEL VIDEO]
