#ifndef ISASERVER_H
#define ISASERVER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PORT 65535
#define MIN_PORT 0

typedef struct {
    char *content;
    struct Entry_t *next;
} Entry_t;

typedef struct {
    char *name;
    Entry_t *first;
} Board_t;

int parse_arguments(int n, char **args, int *port);
bool is_integer(char *str);

#endif