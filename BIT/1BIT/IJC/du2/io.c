/**
 * @file io.c
 * @author Natalia Holkova, FIT
 * @date 15.4.2018
 * @brief Counting word occurrences from input
 *
 * IJC-DU2, b)
 */

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "io.h"

int get_word(char *s, int max, FILE *f) {
    if (f == NULL) {
        fprintf(stderr, "Error! Pointer is NULL.\n");
        return -1;
    }

    int current = 0;
    int c;
    bool inWord = false;
    while ( (c = getc(f)) != EOF && current < max ) {
        if ( isspace(c) || c == '\n') {
            if (inWord) // end of word
                break;
        }
        else
            inWord = true;

        if (inWord) {
            s[current] = c;
            current++;
        }
    }
    s[current] = '\0';

    if (strlen(s) == 0)
        return 1;

    return 0;
}