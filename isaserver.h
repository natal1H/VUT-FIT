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

typedef enum {
    BOARDS = 0,
    BOARD_ADD = 1,
    BOARD_DELETE = 2,
    BOARDS_LIST = 3,
    ITEM_ADD = 4,
    ITEM_DELETE = 5,
    ITEM_UPDATE = 6,
    UNKNOWN = -1
} Command_type;

int parse_arguments(int n, char **args, int *port);
bool is_integer(char *str);
struct addrinfo *get_addr_info(int port);
int bind_listener(struct addrinfo *info);
void resolve(int handler);
void header(int handler, int status);
int get_index(char *str, char c);
Command_type determine_command(char *command);
int strpos(char *hay, char *needle);


#endif