/**
 * @file htab_init.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to initialize hash table
 * IJC-DU2, b)
 */

#include "htab.h"

struct htab_t * htab_init(int size) {
    struct htab_t * tab = malloc(sizeof(struct htab_t) + size * sizeof(struct htab_listitem));
    if (tab == NULL) {
        fprintf(stderr, "Error! Could not allocate memory.\n");
        return NULL;
    }
    tab->arr_size = size;
    tab->size = 0;
    for (int i = 0; i < size; i++) {
        tab->array[i] = NULL;
    }
    return tab;
}