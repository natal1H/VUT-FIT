/**
 * @file htab_size.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to return number of current items in hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

size_t htab_size(struct htab_t *t) {
    return t->size;
}