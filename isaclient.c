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
    ret = get_API_command_args(command_type, command, name, id, content);


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
 * @param type type of API command
 * @param command full API command
 * @param name
 * @param id
 * @param content
 * @return 0 - success, 1 - failure
 */
int get_API_command_args(int type, char *command, char *name, char *id, char *content) {
    if (type == 0) {
        return 0;
    }
    else if (type == 1) {
        char *tmp_name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("board add ")));
        if (tmp_name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            return -1;
        }
        strcpy(tmp_name, command + strlen("board add "));

        // Count number of space to make sure name is valid
        printf("Name: %s\n", tmp_name);
        int spaces = count_space(tmp_name);

        if (spaces > 0) {
            fprintf(stderr, "Error! Name should not contain spaces.\n");
            return 1;
        }
        else {
            return 0;
        }
    }
    else if (type == 2) {
        char *tmp_name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("board delete ")));
        if (tmp_name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            return -1;
        }
        strcpy(tmp_name, command + strlen("board delete "));

        // Count number of space to make sure name is valid
        printf("Name: %s\n", tmp_name);
        int spaces = count_space(tmp_name);

        if (spaces > 0) {
            fprintf(stderr, "Error! Name should not contain spaces.\n");
            return 1;
        }
        else {
            return 0;
        }
    }
    else if (type == 3) {
        char *tmp_name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("boards list ")));
        if (tmp_name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            return -1;
        }
        strcpy(tmp_name, command + strlen("boards list "));

        // Count number of space to make sure name is valid
        printf("Name: %s\n", tmp_name);
        int spaces = count_space(tmp_name);

        if (spaces > 0) {
            fprintf(stderr, "Error! Name should not contain spaces.\n");
            return 1;
        }
        else {
            return 0;
        }
    }

    return 0;
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