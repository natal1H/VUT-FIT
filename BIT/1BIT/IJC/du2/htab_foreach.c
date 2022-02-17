/**
 * @file htab_foreach.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to do something with every item in hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

void htab_foreach( struct htab_t * t, void (*func) (const char *, int *) ) {

    for (unsigned int i = 0; i < t->arr_size; i++) {
        struct htab_listitem *tmp = t->array[i];
        while (tmp != NULL) {
            func(tmp->key, &tmp->data);
            tmp = tmp->next;
        }
    }
}