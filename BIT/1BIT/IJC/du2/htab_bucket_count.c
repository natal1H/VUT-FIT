/**
 * @file htab_bucket_count.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to return size of array in hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

size_t htab_bucket_count(struct htab_t *t) {
    return t->arr_size;
}