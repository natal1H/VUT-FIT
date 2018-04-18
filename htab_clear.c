/**
 * @file htab_clear.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to clear contents of hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

void htab_clear(struct htab_t *t) {

    for (unsigned int i = 0; i < t->arr_size; i++) {
        struct htab_listitem * tmp;
        struct htab_listitem * nextItem = t->array[i];
        t->array[i] = NULL;
        while (nextItem != NULL) {
            tmp = nextItem;
            nextItem = nextItem->next;
            // remove tmp;
            free(tmp->key);
            free(tmp);
        }
    }
    t->size = 0;
}