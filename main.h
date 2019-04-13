/**
 * VUT FIT - IPK - project no. 2
 * Header file for main source code.
 *
 * @file main.h
 * @author Natália Holková (xholko02)
 **/

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tcpIPv4.h"

typedef enum {
    ERR_OK = 0,  // Everything OK
    ERR_PARAMS   // Error with command line arguments, something missing
} ECODES;

typedef struct {
    char *port_range_tcp;
    char *port_range_udp;
    char *interface;
    char *domain_or_ip;
} Params_t;

/**
 * Initialize Params_t structure by setting all attributes to NULL
 * @param params Params_t Structure to be initialized
 */
void init_params(Params_t *params);

/**
 * Check if string represents integer
 * @param str String to check
 * @return Is representing integer?
 */
bool is_integer(char *str);

/**
 * Counts number of occurrences of character in string
 * @param str String where to count character occurrences
 * @param ch Character to find
 * @return Number of occurrences
 */
int count_char_occurrences(char *str, char ch);

#endif