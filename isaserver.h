#ifndef ISASERVER_H
#define ISASERVER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

// Network
#include <netdb.h>
#include <sys/socket.h>

#define MAX_PORT 65535
#define MIN_PORT 0
#define BUF_SIZE 1024

typedef struct {
    char *content;
    struct Item_t *next;
} Item_t;

typedef struct {
    char *name;
    Item_t *first;
} Board_t;


int parse_arguments(int n, char **args, int *port);
bool is_integer(char *str);
struct addrinfo *get_addr_info(int port);
int bind_listener(struct addrinfo *info);
void resolve(int handler);
void header(int handler, int status);

#endif