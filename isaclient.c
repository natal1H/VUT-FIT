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

    Command_type command_type = determine_API_command(command);
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

    Address_t destination = {.host = host, .port = port};
    Command_t api_call = {.type = command_type, .name = name, .id = id, .content = content};

    send_and_get_http_response(&destination, &api_call);

    cleanup(&api_call);
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
        return BOARDS;
    }
    else if (strlen(command) > strlen("board add") && strncmp("board add", command, strlen("board add")) == 0) {
        // board add <name> - POST /boards/<name>
        printf("POST /boards/<name>\n");
        return BOARD_ADD;
    }
    else if (strlen(command) > strlen("board delete") && strncmp("board delete", command, strlen("board delete")) == 0) {
        // board delete <name> - DELETE /boards/<name>
        printf("DELETE /board/<name>\n");
        return BOARD_DELETE;
    }
    else if (strlen(command) > strlen("board list") && strncmp("boards list", command, strlen("board list")) == 0) {
        // boards list <name> - GET /board/<name>
        printf("GET /board/<name>\n");
        return BOARDS_LIST;
    }
    else if (strlen(command) > strlen("item add") && strncmp("item add", command, strlen("item add")) == 0) {
        // item add <name> <content> - POST /board/<name>
        printf("POST /board/<name>\n");
        return ITEM_ADD;
    }
    else if (strlen(command) > strlen("item delete") && strncmp("item delete", command, strlen("item delete")) == 0) {
        // item delete <name> <id> - DELETE /board/<name>/<id>
        printf("DELETE /board/<name>/<id>\n");
        return ITEM_DELETE;
    }
    else if (strlen(command) > strlen("item update") && strncmp("item update", command, strlen("item update")) == 0) {
        // item update <name> <id> <content> - PUT /board/<name>/<id>
        printf("PUT /board/<name>/<id>\n");
        return ITEM_UPDATE;
    }
    else {
        fprintf(stderr, "Error! Unknown API command.\n");
        return UNKNOWN;
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
    if (type == BOARDS) {
        // boards
        return NULL;
    }
    else if (type == BOARD_ADD) {
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
    else if (type == BOARD_DELETE) {
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
    else if (type == BOARDS_LIST) {
        // boards list <name>
        char *name = (char *) malloc(sizeof(char) * (strlen(command) - strlen("board list ")));
        if (name == NULL) {
            fprintf(stderr, "Error while allocating space for name.\n");
            *err = 1;
            return NULL;
        }
        strcpy(name, command + strlen("board list "));

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
    else if (type == ITEM_ADD) {
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
    else if (type == ITEM_DELETE) {
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
    else if (type == ITEM_UPDATE) {
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
    if (type != ITEM_DELETE && type != ITEM_UPDATE) {
        // all except "item delete <name> <id>" and "item update <name> <id> <content>"
        return NULL;
    }
    else if (type == ITEM_DELETE) {
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
    else if (type == ITEM_UPDATE) {
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
    if (type != ITEM_ADD && type != ITEM_UPDATE) {
        // all except "item add <name> <content>" and "item update <name> <id> <content>"
        return NULL;
    }
    else if (type == ITEM_ADD) {
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
    else if (type == ITEM_UPDATE) {
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

/**
 *
 */
void cleanup(Command_t *command) {
    // Clean up command
    free(command->name);
    free(command->id);
    free(command->content);
}

/**
 *
 * @param destination
 * @param command
 * @return
 */
int send_and_get_http_response(Address_t *destination, Command_t *command) {
    printf("Preparing to send HTTP request.\n");

    // Prepare request line
    char *request_line = get_request_line(command);
    printf("Request line: %s\n", request_line);
    if (request_line == NULL) {
        fprintf(stderr, "Error while trying to construct request line.\n");
        return 1;
    }

    char port_str[6];
    sprintf(port_str, "%d", destination->port);

    int clientfd = establish_connection(get_host_info(destination->host, port_str));
    if (clientfd == -1) {
        fprintf(stderr, "Error, failed to connect to host.\n");
        return 1;
    }

    char buf[BUF_SIZE];

    send(clientfd, request_line, strlen(request_line), 0);
    printf("Done sending, waiting for response...\n");

    while (recv(clientfd, buf, BUF_SIZE, 0) > 0) {
        fputs(buf, stdout);
        memset(buf, 0, BUF_SIZE);
    }

    close(clientfd);
    return 0;
}

/**
 *
 * @param command
 * @return
 */
char *get_request_line(Command_t *command) {
    char *request_line = NULL;
    switch (command->type) {
        case BOARDS:
            // Request line: GET /boards HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * strlen("GET /boards HTTP/1.1\r\n\rn"));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "GET /boards HTTP/1.1\r\n\r\n");
            break;
        case BOARD_ADD:
            // Request line: POST /boards/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("POST /boards/") + strlen(command->name) + strlen(" HTTP/1.1\r\n\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "POST /boards/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n\r\n");
            break;
        case BOARD_DELETE:
            // Request line: DELETE /boards/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("DELETE /boards/") + strlen(command->name) + strlen(" HTTP/1.1\r\n\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "DELETE /boards/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n\r\n");
            break;
        case BOARDS_LIST:
            // Request line: GET /board/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("GET /board/") + strlen(command->name) + strlen(" HTTP/1.1\r\n\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "GET /board/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n\r\n");
            break;
        case ITEM_ADD:
            // Request line: POST /board/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("POST /board/") + strlen(command->name) + strlen(" HTTP/1.1\r\n\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "POST /board/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n\r\n");
            break;
        case ITEM_DELETE:
            // Request line: PUT /board/<name>/<id> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("PUT /board/") + strlen(command->name) + 1 + strlen(command->id) + strlen(" HTTP/1.1\r\n\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "PUT /board/");
            strcat(request_line, command->name);
            strcat(request_line, "/");
            strcat(request_line, command->id);
            strcat(request_line, " HTTP/1.1\r\n\r\n");
            break;
        case ITEM_UPDATE:
            // Request line: DELETE /board/<name>/<id> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("DELETE /board/") + strlen(command->name) + 1 + strlen(command->id) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "DELETE /board/");
            strcat(request_line, command->name);
            strcat(request_line, "/");
            strcat(request_line, command->id);
            strcat(request_line, " HTTP/1.1\r\n\r\n");
            break;
        case UNKNOWN:
            return NULL;
    }

    return request_line;
}

struct addrinfo *get_host_info(char *host, char *port) {
    int r;
    struct addrinfo hints, *getaddrinfo_res;

    // Setup hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((r = getaddrinfo(host, port, &hints, &getaddrinfo_res))) {
        fprintf(stderr, "Error getting host info\n");
        return NULL;
    }

    return getaddrinfo_res;
}

int establish_connection(struct addrinfo *info) {
    if (info == NULL) return -1;

    int clientfd;
    for (;info != NULL; info = info->ai_next) {
        if ((clientfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) < 0) {
            fprintf(stderr, "Error establising connection\n");
            continue;
        }

        if (connect(clientfd, info->ai_addr, info->ai_addrlen) < 0) {
            close(clientfd);
            fprintf(stderr, "Error connecting.\n");
            continue;
        }

        freeaddrinfo(info);
        return clientfd;
    }

    return -1;
}