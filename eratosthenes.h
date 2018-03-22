/**
 * @file eratosthenes.h
 * @author Natalia Holkova (xholko02), FIT
 * @date 20.3.2018
 * @brief Hlavickovy subor pre algoritmus Eratosthenovho sita na urcenie prvocisiel
 *
 * Riesenie IJC-DU1, prklad a)
 * Prelozene: gcc 5.4
 */

#include "bit_array.h"
#include <math.h>

#ifndef _ERATOSTHENES_H_
    #define _ERATOSTHENES_H_

    /**
     * @brief Funkcia na urcenie prvocisiel pomocou algoritmu Eratostenovho sita
     *
     * @param pole bitove pole
     */
    void Eratosthenes(bit_array_t pole);

#endif