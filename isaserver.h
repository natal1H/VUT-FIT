#ifndef ISASERVER_H
#define ISASERVER_H

#include "common.h"

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

typedef struct {
    Board_t *first;
} Boards_t;

Boards_t *boards;

int parse_arguments(int n, char **args, int *port);
struct addrinfo *get_addr_info(int port);
int bind_listener(struct addrinfo *info);
void resolve(int handler);
void header(int handler, int status, char *content);
Command_type determine_command(char *command);
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