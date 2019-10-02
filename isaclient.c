#include "isaclient.h"

int main(int argc, char** argv) {

    int port = -1;
    char *host, *command;

    int ret = parse_arguments(argc, argv, &port);
    if (ret != 0) {
        // -h argument or incorrect usage - quit program
        return (ret == 1 ? 0 : 1);
    }

    host = argv[2];
    command = get_command(argc, argv);
    printf("Host: %s\n", host);
    printf("Port: %d\n", port);
    printf("Command: %s\n", command);

    int command_type = determine_API_command(command);
    printf("Command type: %d\n", command_type);

    if (command_type == -1) {
        return 1;
    }

    char *name = NULL, *id = NULL, *content = NULL;

    name = get_API_command_arg_name(command_type, command, &ret);
    if (ret != 0) {
        fprintf(stderr, "Error in command structure.\n");
        free(command);
        return 1;
    }

    id = get_API_command_arg_id(command_type, command, &ret);
    if (ret != 0) {
        fprintf(stderr, "Error in command structure.\n");
        free(command);
        return 1;
    }

    content = get_API_command_arg_content(command_type, command, &ret);
    if (ret != 0) {
        fprintf(stderr, "Error in command structure.\n");
        free(command);
        return 1;
    }

    printf("<name>: %s\n", name);
    printf("<id>: %s\n", id);
    printf("<content>: %s\n", content);

    free(command);

    return 0;
}

/**
 *
 * @param n
 * @param args
 * @param port
 * @param host
 * @param command
 * @return
 */
int parse_arguments(int n, char **args, int *port) {
    if (n == 2 && strcmp(args[1], "-h") == 0) {
        // -h argument: display usage
        printf("HTTP dashboard client\n");
        printf("Usage: ./isaclinet -H <host> -p <port> <command>\n");
        return 1;
    }
    else if (n >= 6 && strcmp(args[1], "-H") == 0 && strcmp(args[3], "-p") == 0) {
        // Attempting to select host and port - check if valid port
        if (is_integer(args[4])) {
            // Get port as an integer
            int tmp = atoi(args[4]);

            // Check port range
            if (tmp >= MIN_PORT && tmp <= MAX_PORT) {
                // Valid port number
                *port = tmp;
                return 0;
            }
            else {
                fprintf(stderr, "Error! Invalid port number.\n");
                return -1;
            }

        }
        else {
            fprintf(stderr, "Error! Incorrect program usage.\n");
            return -1;
        }
    }
    else {
        fprintf(stderr, "Error! Incorrect program usage.\n");
        return -1;
    }
}

/**
 *
 * @param str
 * @return True if represents integer, false if not
 */
bool is_integer(char *str) {
    for (int i = 0; i < strlen(str); i++)
        if (str[i] < '0' || str[i] > '9')
            return false;

    return true;
}

/**
 *
 * @param n
 * @param args
 * @return
 */
char *get_command(int n, char **args) {
    // First count total length of command
    int length = 0;
    for (int i = 5; i < n; i++) {
        for (int j = 0; j < strlen(args[i]); j++) {
            length++;
        }
        // + count the spaces
        length++;
    }
    length--;
    printf("Command total length: %d\n", length);

    char *command = (char *) malloc(sizeof(char) * length);
    if (command == NULL) {
        fprintf(stderr, "Error while allocating space for command.\n");
        return NULL;
    }

    int index = 0;
    for (int i = 5; i < n; i++) {
        for (int j = 0; j < strlen(args[i]); j++) {
            command[index++] = args[i][j];
        }
        // + add the space
        command[index++] = ' ';
    }
    command[--index] = '\0';
    return command;
}

/**
 *
 * @param command
 * @return 0 - board, 1 - board add, 2 - board delete, 3 - boards list, 4 - item add, 5 - item delete, 6 - item update, -1 - else
 */
int determine_API_command(char *command) {

    if (strlen(command) == strlen("boards") && strcmp("boards", command) == 0) {
        // boards - GET /boards
        printf("GET /boards\n");
        return 0;
    }
    else if (strlen(command) > strlen("board add") && strncmp("board add", command, strlen("board add")) == 0) {
        // board add <name> - POST /boards/<name>
        printf("POST /boards/<name>\n");
        return 1;
    }
    else if (strlen(command) > strlen("board delete") && strncmp("board delete", command, strlen("board delete")) == 0) {
        // board delete <name> - DELETE /boards/<name>
        printf("DELETE /board/<name>\n");
        return 2;
    }
    else if (strlen(command) > strlen("boards list") && strncmp("boards list", command, strlen("boards list")) == 0) {
        // boards list <name> - GET /board/<name>
        printf("GET /board/<name>\n");
        return 3;
    }
    else if (strlen(command) > strlen("item add") && strncmp("item add", command, strlen("item add")) == 0) {
        // item add <name> <content> - POST /board/<name>
        printf("POST /board/<name>\n");
        return 4;
    }
    else if (strlen(command) > strlen("item delete") && strncmp("item delete", command, strlen("item delete")) == 0) {
        // item delete <name> <id> - DELETE /board/<name>/<id>
        printf("DELETE /board/<name>/<id>\n");
        return 5;
    }
    else if (strlen(command) > strlen("item update") && strncmp("item update", command, strlen("item update")) == 0) {
        // item update <name> <id> <content> - PUT /board/<name>/<id>
        printf("PUT /board/<name>/<id>\n");
        return 6;
    }
    else {
        fprintf(stderr, "Error! Unknown API command.\n");
        return -1;
    }
}

/**
 *
 * @param type
 * @param command
 * @param err
 * @return
 */
char *get_API_command_arg_name(int type, char *command, int *err) {
    *err = 0;
    if (type == 0) {
        // boards
        return NULL;
    }
    else if (type == 1) {
        // board add <name>
        char *name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("board add ")));
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strcpy(name, command + strlen("board add "));

        // Count number of space to make sure name is valid
        int spaces = count_space(name);

        if (spaces > 0) {
            fprintf(stderr, "Error! Name should not contain spaces.\n");
            *err = 1;
            free(name);
            return NULL;
        }
        else {
            return name;
        }
    }
    else if (type == 2) {
        // board delete <name>
        char *name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("board delete ")));
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strcpy(name, command + strlen("board delete "));

        // Count number of space to make sure name is valid
        int spaces = count_space(name);

        if (spaces > 0) {
            fprintf(stderr, "Error! Name should not contain spaces.\n");
            *err = 1;
            free(name);
            return NULL;
        }
        else {
            return name;
        }
    }
    else if (type == 3) {
        // boards list <name>
        char *name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("boards list ")));
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strcpy(name, command + strlen("boards list "));

        // Count number of space to make sure name is valid
        int spaces = count_space(name);

        if (spaces > 0) {
            fprintf(stderr, "Error! Name should not contain spaces.\n");
            *err = 1;
            free(name);
            return NULL;
        }
        else {
            return name;
        }
    }
    else if (type == 4) {
        // item add <name> <content>
        // determine length of name (find index of first space after "item add "
        int name_length = get_index(command + strlen("item add "), ' ');
        if (name_length == -1) {
            fprintf(stderr, "Error! No content after <name>\n");
            *err = 1;
            return NULL;
        }

        char *name = (char *) malloc(sizeof(char) * name_length);
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strncpy(name, command + strlen("item add "), name_length);

        return name;
    }
    else if (type == 5) {
        // item delete <name> <id>
        // determine length of name (find index of first space after "item delete "
        int name_length = get_index(command + strlen("item delete "), ' ');
        if (name_length == -1) {
            fprintf(stderr, "Error! No content after <name>\n");
            *err = 1;
            return NULL;
        }

        char *name = (char *) malloc(sizeof(char) * name_length);
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strncpy(name, command + strlen("item delete "), name_length);

        return name;
    }
    else if (type == 6) {
        // item update <name> <id>
        // determine length of name (find index of first space after "item update "
        int name_length = get_index(command + strlen("item update "), ' ');
        if (name_length == -1) {
            fprintf(stderr, "Error! No content after <name>\n");
            *err = 1;
            return NULL;
        }

        char *name = (char *) malloc(sizeof(char) * name_length);
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strncpy(name, command + strlen("item update "), name_length);

        return name;
    }

    return NULL;
}

/**
 *
 * @param type
 * @param command
 * @param err
 * @return
 */
char *get_API_command_arg_id(int type, char *command, int *err) {
    *err = 0;
    if (type != 5 && type != 6) {
        // all except "item delete <name> <id>" and "item update <name> <id> <content>"
        return NULL;
    }
    else if (type == 5) {
        // item delete <name> <id>

        int name_length = get_index(command + strlen("item delete "), ' ');
        char *id = (char *) malloc(sizeof(char) * (strlen(command) - strlen("item delete ") - name_length - 1));
        if (id == NULL) {
            fprintf(stderr, "Error while allocating space of id.\n");
            *err = 1;
            return NULL;
        }

        strcpy(id, command + strlen("item delete ") + name_length + 1);

        // Count spaces in id (should be 0) and check if it represents an integer
        if (count_space(id) > 0 || !is_integer(id)) {
            fprintf(stderr, "Error in command structure.\n");
            *err = 1;
            free(id);
            return NULL;
        }

        return id;
    }
    else if (type == 6) {
        // item update <name> <id> <content>

        int name_length = get_index(command + strlen("item update "), ' ');
        int id_length = get_index(command + strlen("item update ") + name_length + 1, ' ');
        char *id = (char *) malloc(sizeof(char) * id_length);
        if (id == NULL) {
            fprintf(stderr, "Error while allocating space of id.\n");
            *err = 1;
            return NULL;
        }

        strncpy(id, command + strlen("item update ") + name_length + 1, id_length);

        // Check if it represents an integer
        if (!is_integer(id)) {
            fprintf(stderr, "Error in command structure.\n");
            *err = 1;
            free(id);
            return NULL;
        }

        return id;
    }

    return NULL;
}

/**
 *
 * @param type
 * @param command
 * @param err
 * @return
 */
char *get_API_command_arg_content(int type, char *command, int *err) {
    *err = 0;
    if (type != 4 && type != 6) {
        // all except "item add <name> <content>" and "item update <name> <id> <content>"
        return NULL;
    }
    else if (type == 4) {
        // item add <name> <content>

        int name_length = get_index(command + strlen("item add "), ' ');
        char *content = (char *) malloc(sizeof(char) * (strlen(command) - strlen("item add ") - name_length - 1));
        if (content == NULL) {
            fprintf(stderr, "Error while allocating space of content.\n");
            *err = 1;
            return NULL;
        }

        strcpy(content, command + strlen("item add ") + name_length + 1);

        return content;
    }
    else if (type == 6) {
        // item update <name> <id> <content>

        int name_length = get_index(command + strlen("item update "), ' ');
        int id_length = get_index(command + strlen("item update ") + name_length + 1, ' ');
        char *content = (char *) malloc(sizeof(char) * (strlen(command) - strlen("item update ") - 1 - name_length - 1 - id_length - 1));
        if (content == NULL) {
            fprintf(stderr, "Error while allocating space of content.\n");
            *err = 1;
            return NULL;
        }

        strcpy(content, command + strlen("item update ") + name_length + 1 + id_length + 1);

        return content;
    }

    return NULL;
}

/**
 *
 * @param str
 * @return
 */
int count_space(char *str) {
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
        if (str[i] == ' ') count++;

    return count;
}

/**
 *
 * @param str
 * @param c
 * @return
 */
int get_index(char *str, char c) {
    char *tmp = str;
    int index = 0;
    while (*tmp++ != c && index < strlen(str)) index++;
    return (index == strlen(str) ? -1 : index);
}