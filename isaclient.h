#ifndef ISACLIENT_H
#define ISACLIENT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PORT 65535
#define MIN_PORT 0

int parse_arguments(int n, char **args, int *port);
bool is_integer(char *str);
char *get_command(int n, char **args);
int determine_API_command(char *command);
char *get_API_command_arg_name(int type, char *command, int *err);
char *get_API_command_arg_id(int type, char *command, int *err);
char *get_API_command_arg_content(int type, char *command, int *err);
int count_space(char *str);
int get_index(char *str, char c);

#endif