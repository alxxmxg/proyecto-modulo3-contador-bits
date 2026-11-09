// ============================================================
// PROYECTO MÓDULO 3: ACELERADOR DE FUNCIONES
// Problema: Contador de Bits (Population Count)
// ============================================================
//
// OBJETIVO:
//   Implementar en ensamblador inline (asm volatile) la función
//   contar_bits_asm(), que cuenta cuántos bits en 1 tiene un
//   número de 64 bits, y comparar su rendimiento con la versión
//   en C puro.
//
// COMPILAR:
//   gcc main.c -O0 -o acelerador.exe
//
// PRUEBA RÁPIDA (con pocos datos, antes de usar 100 millones):
//   gcc main.c -O0 -DN=1000000 -o acelerador.exe
//
// ============================================================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ------------------------------------------------------------
// CONFIGURACIÓN
// ------------------------------------------------------------
#ifndef N
#define N 100000000        // 100 millones de iteraciones (~800 MB de RAM)
#endif

// Selecciona qué versión en ASM se usa:
//   0 = bucle optimizado (n & (n-1))
//   1 = instrucción POPCNT (Fase 2, requiere CPU con soporte)
#define USAR_POPCNT 1

// ------------------------------------------------------------
// VERSIÓN EN C PURO
// ------------------------------------------------------------
// Revisa los 64 bits uno por uno: toma el bit menos significativo,
// lo suma al contador y desplaza n a la derecha hasta que sea 0.
int contar_bits_c(unsigned long long n) {
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

// ------------------------------------------------------------
// VERSIÓN EN ENSAMBLADOR INLINE
// ------------------------------------------------------------
// ENTRADA: n = número de 64 bits (%1)
// SALIDA:  cantidad de bits en 1 (%0)
//
#if USAR_POPCNT == 0

// Algoritmo de Kernighan: en cada vuelta, n & (n-1) apaga el bit
// en 1 más bajo. Por eso el bucle solo itera una vez por cada bit
// en 1, en lugar de recorrer los 64 bits.
int contar_bits_asm(unsigned long long n) {
    int count;
    asm volatile (
        "xor %%eax, %%eax\n\t"        // EAX = 0 (contador de bits)
        "mov %1, %%rcx\n\t"           // RCX = n
        "test %%rcx, %%rcx\n\t"       // ¿n == 0?
        "jz 2f\n\t"                   // si es 0, no hay bits: terminar

        "1:\n\t"                      // inicio del bucle
        "inc %%eax\n\t"               // contador++
        "lea -1(%%rcx), %%rdx\n\t"    // RDX = n - 1
        "and %%rdx, %%rcx\n\t"        // n = n & (n-1): apaga el bit 1 más bajo
        "jnz 1b\n\t"                  // si n != 0, repetir

        "2:\n\t"                      // salida
        "mov %%eax, %0\n\t"           // count = EAX
        : "=r" (count)                // %0: salida
        : "r" (n)                     // %1: entrada
        : "%rax", "%rcx", "%rdx", "cc"  // registros y banderas modificados
    );
    return count;
}

#else

// POPCNT: instrucción de hardware que cuenta los bits en 1 de un
// registro de 64 bits en una sola operación (sin bucle).
// %q0 fuerza el registro de salida a 64 bits, igual que la entrada.
int contar_bits_asm(unsigned long long n) {
    int count;
    asm volatile (
        "popcnt %1, %q0\n\t"          // count = número de bits en 1 de n
        : "=r" (count)                // %0: salida
        : "r" (n)                     // %1: entrada
        : "cc"                        // POPCNT modifica las banderas
    );
    return count;
}

#endif

// ------------------------------------------------------------
// FUNCIÓN PRINCIPAL
// ------------------------------------------------------------
int main() {
    // Semilla fija para que los resultados sean reproducibles
    srand(42);

    // Crear arreglo de números aleatorios de 64 bits
    unsigned long long *arr =
        (unsigned long long *)malloc((size_t)N * sizeof(unsigned long long));
    if (arr == NULL) {
        printf("ERROR: no hay memoria suficiente. Reduce el valor de N.\n");
        return 1;
    }
    for (int i = 0; i < N; i++) {
        arr[i] = ((unsigned long long)rand() << 32) | rand();
    }

    // ---- Medir tiempo en C ----
    clock_t inicio = clock();
    long long total_c = 0;
    for (int i = 0; i < N; i++) {
        total_c += contar_bits_c(arr[i]);
    }
    clock_t fin = clock();
    double tiempo_c = (double)(fin - inicio) / CLOCKS_PER_SEC;

    // ---- Medir tiempo en ASM ----
    inicio = clock();
    long long total_asm = 0;
    for (int i = 0; i < N; i++) {
        total_asm += contar_bits_asm(arr[i]);
    }
    fin = clock();
    double tiempo_asm = (double)(fin - inicio) / CLOCKS_PER_SEC;

    // ---- Verificar que los resultados son iguales ----
    printf("Total en C:   %lld\n", total_c);
    printf("Total en ASM: %lld\n", total_asm);
    if (total_c == total_asm) {
        printf("OK: Los resultados coinciden.\n");
    } else {
        printf("ERROR: Los resultados NO coinciden.\n");
    }

    // ---- Mostrar tiempos ----
    printf("\nVersion ASM: %s\n", USAR_POPCNT ? "POPCNT" : "bucle n & (n-1)");
    printf("Tiempo en C:   %.4f s\n", tiempo_c);
    printf("Tiempo en ASM: %.4f s\n", tiempo_asm);
    if (tiempo_asm > 0) {
        printf("Mejora: %.2fx\n", tiempo_c / tiempo_asm);
    }

    free(arr);
    return 0;
}