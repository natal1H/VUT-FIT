/**
 * @file htab_move.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to move contents of hash table to another
 *
 * IJC-DU2, b)
 */

#include "htab.h"

struct htab_t * htab_move(int newsize, struct htab_t * t2) {
    struct htab_t * tab = malloc(sizeof(struct htab_t) + newsize * sizeof(struct htab_listitem));
    if (tab == NULL) {
        fprintf(stderr, "Error! Could not allocate memory.\n");
        return NULL;
    }
    tab->arr_size = newsize;
    tab->size = t2->size;

    for (unsigned int i = 0; i < t2->arr_size; i++) {
        struct htab_listitem * tmp = t2->array[i];
        while (tmp != NULL) {
            htab_lookup_add(tab, tmp->key);
            tmp = tmp->next;
        }
    }

    htab_clear(t2);

    return tab;
}