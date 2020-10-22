/**
 * @file htab_find.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to find item by key in hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

struct htab_listitem * htab_find(struct htab_t *t, const char *key) {
    unsigned int hash = htab_hash_function(key);
    struct htab_listitem *foundItem;
    struct htab_listitem *nextItem = NULL;

    nextItem = t->array[ hash % t->arr_size ];

    while( nextItem != NULL && nextItem->key != NULL && strcmp( key, nextItem->key ) > 0 ) {
        nextItem = nextItem->next;
    }
    if( nextItem != NULL && nextItem->key != NULL && strcmp( key, nextItem->key ) == 0 ) {
        foundItem = nextItem;
    }
    else {
        foundItem = NULL;
    }

    return foundItem;
}
