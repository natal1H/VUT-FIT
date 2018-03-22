/**
 * @file primes.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief Vypise poslednych 10 prvocisiel
 *
 * Riesenie IJC-DU1, prklad a)
 * Prelozene: gcc 5.4
 */

#include "eratosthenes.h"
#include <stdio.h>

/**
 * @brief Vypise vzostupne poslednych 10 prvocisiel
 *
 * @param pole bitove pole s prvocislami na indexoch, ktorych hodnota je 0
 */
void printLast10Primes(bit_array_t pole) {
    unsigned int not_found_primes = 10;
    unsigned long N = bit_array_size(pole);

    unsigned long i = N-1;
    /* Cyklus na najdenie 10. najvacsieho prvocisla */
    while (i > 0 && not_found_primes > 0) {
        if (bit_array_getbit(pole, i) == 0 )
                not_found_primes--;
        i--;
    }
    /* Cyklus na vzostupny vypis prvocisiel */
    while (i< N) {
        if (bit_array_getbit(pole, i) == 0 )
            printf("%lu\n", i);
	i++;
    }

}

int main() {
    //bit_array_create(p, 100);
    //bit_array_create(p, 100000L);
    bit_array_create(p, 222000000L);

    Eratosthenes(p);
    printLast10Primes(p);

    return 0;
}
