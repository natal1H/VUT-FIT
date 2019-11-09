/**
 * @file common.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 18.11.2019
 * @brief Implements function that both client and server use
 *
 * Project for ISA, task variant - HTTP dashboard
 */


#ifndef COMMON_H
#define COMMON_H

// General
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
#define BUF_SIZE 4096

typedef enum {
    BOARDS = 0,
    BOARD_ADD = 1,
    BOARD_DELETE = 2,
    BOARD_LIST = 3,
    ITEM_ADD = 4,
    ITEM_DELETE = 5,
    ITEM_UPDATE = 6,
    UNKNOWN = -1
} Command_type;

bool is_integer(char *str);
int get_index(char *str, char c);
int strpos(char *hay, char *needle);

#endif
