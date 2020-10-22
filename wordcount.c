/**
 * @file wordcount.c
 * @author Natalia Holkova, FIT
 * @date 15.4.2018
 * @brief Counting word occurrences from input
 *
 * IJC-DU2, b)
 */

#include "io.h"
#include "htab.h"

#define MAX_WORD 127

/**
 * @brief Prints pair key - word and value - number of occurences
 * @param key Word
 * @param value Number of occurences of word
 */
void print_pair(const char *key, int *value) {
    printf("%s %d\n", key, *value);
}

int main() {
    FILE *f = stdin;

    struct htab_t *table = htab_init(10);
    char word[MAX_WORD];

    /* Read words until EOF is read */
    while ( get_word(word, MAX_WORD, f) != 1 ) {
        htab_lookup_add(table, word); // Attempt to add word to hash table
    }

    htab_foreach(table, print_pair); // Print all key - value pairs

    htab_free(table); // Free table

    return 0;
}