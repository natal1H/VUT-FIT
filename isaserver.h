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
#define BUF_SIZE 4096
#define MAX_ITEMS 512

typedef struct Item_t Item_t;

struct Item_t {
    struct Item_t *next;
    char *content;
};

typedef struct Board_t Board_t;

struct Board_t {
    char *name;
    Item_t *first;
    struct Board_t *next;
};

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

typedef struct {
    Board_t *first;
} Boards_t;

Boards_t *boards;

int parse_arguments(int n, char **args, int *port);
bool is_integer(char *str);
struct addrinfo *get_addr_info(int port);
int bind_listener(struct addrinfo *info);
void resolve(int handler);
void header(int handler, int status, char *content);
int get_index(char *str, char c);
Command_type determine_command(char *command);
int strpos(char *hay, char *needle);
char *get_command_arg_name(Command_type type, char *command, int *err);
char *get_command_arg_id(Command_type type, char *command, int *err);
char *run_command(Command_type type, char *name, char *id, char *content, int *err);
Boards_t *init_boards();
Board_t *create_board(char *name);
int add_board(Boards_t *boards, Board_t *board);
int delete_board(Boards_t *boards, char *name);
char *list_boards(Boards_t *boards);
char *list_board(Boards_t *boards, char *name);
int item_add(Boards_t *boards, char *name, char *content);
int item_delete(Boards_t *boards, char *name, char *id);
int item_update(Boards_t *boards, char *name, char *id, char *content);

#endif