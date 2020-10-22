/**
 * @file htab_lookup_add.c
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Implements function to look up and add item to hash table
 *
 * IJC-DU2, b)
 */

#include "htab.h"

struct htab_listitem * htab_lookup_add(struct htab_t *t, const char *key) {
    unsigned int hash = htab_hash_function(key);
    struct htab_listitem *newItem = NULL;
    struct htab_listitem *nextItem = NULL;
    struct htab_listitem *lastItem = NULL;

    nextItem = t->array[ hash % t->arr_size ];

    while( nextItem != NULL && nextItem->key != NULL && strcmp( key, nextItem->key ) > 0 ) {
        lastItem = nextItem;
        nextItem = nextItem->next;
    }
    if( nextItem != NULL && nextItem->key != NULL && strcmp( key, nextItem->key ) == 0 ) {
        nextItem->data++;
        newItem = nextItem;
    }
    else {
        newItem = malloc(sizeof(struct htab_listitem));
        if (newItem == NULL) {
            fprintf(stderr, "Error! Could not allocate memory.\n");
            return NULL;
        }
        newItem->data = 1;

        newItem->key = malloc(sizeof(char)*strlen(key));
        if (newItem->key == NULL) {
            fprintf(stderr, "Error! Could not allocate memory.\n");
            return NULL;
        }
        strcpy(newItem->key, key);
        newItem->next = NULL;
        /* We're at the start of the linked list in this bin. */
        if ( nextItem == t->array[hash % t->arr_size] ) {
            newItem->next = nextItem;
            t->array[hash % t->arr_size] = newItem;
            t->size = t->size + 1;

            /* We're at the end of the linked list in this bin. */
        } else if ( nextItem == NULL ) {
            lastItem->next = newItem;

            /* We're in the middle of the list. */
        } else  {
            newItem->next = nextItem;
            lastItem->next = newItem;
        }
    }

    return newItem;
}
