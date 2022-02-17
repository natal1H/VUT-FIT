/**
 * @file htab_remove.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to remove item by key from hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

bool htab_remove(struct htab_t *t, const char *key) {
    unsigned int hash = htab_hash_function(key);
    struct htab_listitem *nextItem = NULL;
    struct htab_listitem *lastItem = NULL;

    nextItem = t->array[ hash % t->arr_size ];

    while( nextItem != NULL && nextItem->key != NULL && strcmp( key, nextItem->key ) > 0 ) {
        lastItem = nextItem;
        nextItem = nextItem->next;
    }
    if( nextItem != NULL && nextItem->key != NULL && strcmp( key, nextItem->key ) == 0 ) {
        // FOUND -> need to remove ( Found is nextItem )

        // Remove from start
        if (nextItem == t->array[hash % t->arr_size]) {
            t->array[hash % t->arr_size] = nextItem->next;
            free(nextItem->key);
            free(nextItem);
        }
        // Remove from end
        else if (nextItem->next == NULL) {
            lastItem->next = NULL;
            free(nextItem->key);
            free(nextItem);
        }
        // Remove from middle
        else {
            lastItem->next = nextItem->next;
            free(nextItem->key);
            free(nextItem);
        }
        return true;
    }
    else {
        // NOT FOUND
        return false;
    }
}
