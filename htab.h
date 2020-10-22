/**
 * @file htab.h
 * @author Natalia Holkova, FIT
 * @date 16.4.2018
 * @brief Interface for hash table
 *
 * IJC-DU2, b)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Structure record in hash table
 */
struct htab_listitem {
    char *key;
    int data;
    struct htab_listitem *next;
};

/**
 * @brief Structure hash table
 */
struct htab_t {
    size_t size;
    size_t arr_size;
    struct htab_listitem * array[];
};

/**
 *
 * @param str
 * @return
 */
unsigned int htab_hash_function(const char *str);

/**
 *
 * @param size
 * @return
 */
struct htab_t * htab_init(int size);

/**
 *
 * @param size
 * @param t2
 * @return
 */
struct htab_t * htab_move(int size, struct htab_t * t2);

/**
 *
 * @param t
 * @return
 */
size_t htab_size(struct htab_t *t);

/**
 *
 * @param t
 * @return
 */
size_t htab_bucket_count(struct htab_t *t);

/**
 *
 * @param t
 * @param key
 * @return
 */
struct htab_listitem * htab_lookup_add(struct htab_t *t, const char *key);

/**
 *
 * @param t
 * @param key
 * @return
 */
struct htab_listitem * htab_find(struct htab_t *t, const char *key);

/**
 *
 * @param t
 * @param key
 * @return
 */
bool htab_remove(struct htab_t *t, const char *key);

/**
 *
 * @param t
 */
void htab_clear(struct htab_t *t);

/**
 *
 * @param t
 */
void htab_free(struct htab_t *t);

/**
 *
 * @param t
 * @param func
 */
void htab_foreach( struct htab_t * t, void (*func) (const char *, int *) );