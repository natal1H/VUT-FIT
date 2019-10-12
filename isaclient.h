#ifndef ISACLIENT_H
#define ISACLIENT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

// Network
#include <sys/socket.h>
#include <netdb.h>

#define MAX_PORT 65535
#define MIN_PORT 0
#define SRC_PORT 12345
#define BUF_SIZE 1024

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

typedef struct {
    char *host;
    int port;
} Address_t;

typedef struct {
    Command_type type;
    char *name;
    char *id;
    char *content;
} Command_t;

int parse_arguments(int n, char **args, int *port);
bool is_integer(char *str);
char *get_command(int n, char **args);
Command_type determine_API_command(char *command);
char *get_API_command_arg_name(Command_type type, char *command, int *err);
char *get_API_command_arg_id(Command_type type, char *command, int *err);
char *get_API_command_arg_content(Command_type type, char *command, int *err);
int count_space(char *str);
int get_index(char *str, char c);
void cleanup(Command_t *command);
int send_and_get_http_response(Address_t *destination, Command_t *command);
char *get_request_line(Command_t *command);
struct addrinfo *get_host_info(char *host, char *port);
int establish_connection(struct addrinfo *info);

#endif