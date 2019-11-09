/**
 * @file isaclient.h
 * @author Natalia Holkova (xholko02), FIT
 * @date 18.11.2019
 * @brief Implements client for sending HTTP requests
 *
 * Project for ISA, task variant - HTTP dashboard
 */

#ifndef ISACLIENT_H
#define ISACLIENT_H

#include "common.h"

#define SRC_PORT 12345

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
char *get_command(int n, char **args);
Command_type determine_API_command(char *command);
char *get_API_command_arg_name(Command_type type, char *command, int *err);
char *get_API_command_arg_id(Command_type type, char *command, int *err);
char *get_API_command_arg_content(Command_type type, char *command, int *err);
int count_space(char *str);
void cleanup(Command_t *command);
int send_and_get_http_response(Address_t *destination, Command_t *command);
char *get_request_line(Command_t *command);
char *get_host_header(Address_t *destination);
char *get_content_header(Command_t *command);
char *get_message_body(Command_t *command);
struct addrinfo *get_host_info(char *host, char *port);
int establish_connection(struct addrinfo *info);

#endif
