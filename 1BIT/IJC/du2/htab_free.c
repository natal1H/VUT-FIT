/**
 * @file htab_free.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to free hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

void htab_free(struct htab_t * t) {
    htab_clear(t);
    free(t);
    t = NULL;
}