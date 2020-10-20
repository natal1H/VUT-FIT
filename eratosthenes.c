/**
 * @file eratosthenes.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief Implementuje algoritmus Eratosthenovho sita na urcenie prvocisiel
 *
 * Riesenie IJC-DU1, prklad a)
 * Prelozene: gcc 5.4
 */

#include "bit_array.h"
#include <math.h>

void Eratosthenes(bit_array_t pole) {
    bit_array_setbit(pole, 0, 1);
    bit_array_setbit(pole, 1, 1);

    unsigned long N = bit_array_size(pole); // N = velkost pola
    unsigned long limit = sqrt(N);

    /* Nastavenie vsetkych nasobkov 2 na 1 */
    for (unsigned long i = 4; i < N; i += 2)
        bit_array_setbit(pole, i, 1);

    for (unsigned long i = 3; i <= limit; i += 2) {
        if (bit_array_getbit(pole, i) == 0) { // i je prvocislo
            /* nastavenie vsetkych nasobkov i na 1 (nie su prvocislami) */
            for (unsigned long j = i * i; j < N; j += 2 * i) {
                if (bit_array_getbit(pole, j) == 0)
                    bit_array_setbit(pole, j, 1); // nastavenie nasobku i na 1 (nie je prvocislo)
            }
        }
    }
}
