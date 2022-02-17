/**
 * @file htab_hash_function.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements hash function
 *
 * IJC-DU2, b)
 */

#include "htab.h"

unsigned int htab_hash_function(const char *str) {
    unsigned int h = 0; // 32bit
    const unsigned char *p;
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}