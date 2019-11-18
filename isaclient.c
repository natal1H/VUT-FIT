/**
 * @file isaclient.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 18.11.2019
 * @brief Implements client for sending HTTP requests
 *
 * Project for ISA, task variant - HTTP dashboard
 */

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
    Command_type command_type = determine_API_command(command);

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

    // Check if name only contains numbers and letters
    if (name != NULL && !check_name_validity(name)) {
        fprintf(stderr, "Error! Board name %s is invalid.\n", name);
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

    free(command);

    Address_t destination = {.host = host, .port = port};
    Command_t api_call = {.type = command_type, .name = name, .id = id, .content = content};
    send_and_get_http_response(&destination, &api_call);

    cleanup(&api_call);
    return 0;
}

 /**
  * Function to parse command line arguments
  *
  * @param n Number of command line arguments
  * @param args Array containing the command line arguments
  * @param port Pointer to integer where to store port number
  * @return 0 - normal usage, 1 - help argument, -1 - bad usage
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
 * Function to get the API command as string
 *
 * @param n Number of command arguments
 * @param args Array containing command line arguments
 * @return String containing the API command or NULL if error occurred
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
 * Function to determine the type of API command
 *
 * @param command String containing the whole command
 * @return Type as defined in Command_type enum
 */
int determine_API_command(char *command) {

    if (strlen(command) == strlen("boards") && strcmp("boards", command) == 0) {
        // boards - GET /boards
        return BOARDS;
    }
    else if (strlen(command) > strlen("board add") && strncmp("board add", command, strlen("board add")) == 0) {
        // board add <name> - POST /boards/<name>
        return BOARD_ADD;
    }
    else if (strlen(command) > strlen("board delete") && strncmp("board delete", command, strlen("board delete")) == 0) {
        // board delete <name> - DELETE /boards/<name>
        return BOARD_DELETE;
    }
    else if (strlen(command) > strlen("board list") && strncmp("board list", command, strlen("board list")) == 0) {
        // boards list <name> - GET /board/<name>
        return BOARD_LIST;
    }
    else if (strlen(command) > strlen("item add") && strncmp("item add", command, strlen("item add")) == 0) {
        // item add <name> <content> - POST /board/<name>
        return ITEM_ADD;
    }
    else if (strlen(command) > strlen("item delete") && strncmp("item delete", command, strlen("item delete")) == 0) {
        // item delete <name> <id> - DELETE /board/<name>/<id>
        return ITEM_DELETE;
    }
    else if (strlen(command) > strlen("item update") && strncmp("item update", command, strlen("item update")) == 0) {
        // item update <name> <id> <content> - PUT /board/<name>/<id>
        return ITEM_UPDATE;
    }
    else {
        fprintf(stderr, "Error! Unknown API command.\n");
        return UNKNOWN;
    }
}

/**
 * Function to get <name> argument from API command
 *
 * @param type Type of API command
 * @param command String containing the whole command
 * @param err Pointer to integer storing error code
 * @return Name argument
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
    else if (type == BOARD_LIST) {
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
 * Function to get <id> argument from API command
 *
 * @param type Type of API command
 * @param command String containing the whole command
 * @param err Pointer to integer storing error code
 * @return ID argument
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
    else {
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
 * Function to get <content> argument from API command
 *
 * @param type Type of API command
 * @param command String containing the whole command
 * @param err Pointer to integer storing error code
 * @return Content argument
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
    else {
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
 * Function to count number of spaces in string
 *
 * @param str Input string
 * @return Number of spaces inside the string
 */
int count_space(char *str) {
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
        if (str[i] == ' ') count++;

    return count;
}

/**
 * Clean up function
 */
void cleanup(Command_t *command) {
    // Clean up command
    free(command->name);
    free(command->id);
    free(command->content);
}

/**
 * Function to send HTTP request and recieve response
 *
 * @param destination Where to send HTTP request (host and port)
 * @param command Command structure containing all command arguments and type
 * @return Success or failure
 */
int send_and_get_http_response(Address_t *destination, Command_t *command) {
    // Prepare request line
    char *request_line = get_request_line(command);
    if (request_line == NULL) {
        fprintf(stderr, "Error while trying to construct request line.\n");
        return 1;
    }

    // Prepare headers
    char *host_header =  get_host_header(destination); // Host header
    char *content_header = get_content_header(command); // Content header - if there is content to send
    char *message_body = get_message_body(command); // Message body - if there is content to send

    char port_str[6];
    sprintf(port_str, "%d", destination->port);

    char *request = (char *) malloc(sizeof(char) * (strlen(request_line) + strlen(host_header) + (content_header == NULL ? 0 : strlen(content_header))
            + strlen("\r\n") + (message_body == NULL ? 0 : strlen(message_body))));
    if (request == NULL) {
        return 1;
    }
    strcpy(request, request_line);
    strcat(request, host_header);
    if (content_header != NULL)
        strcat(request, content_header);
    strcat(request, "\r\n");
    if (message_body != NULL)
        strcat(request, message_body);

    int clientfd = establish_connection(get_host_info(destination->host, port_str));
    if (clientfd == -1) {
        fprintf(stderr, "Error, failed to connect to host.\n");
        return 1;
    }
    char buf[BUF_SIZE];
    send(clientfd, request, strlen(request), 0);

    char response[BUF_SIZE];
    while (recv(clientfd, buf, BUF_SIZE, 0) > 0) {
        strcat(response, buf);
        memset(buf, 0, BUF_SIZE);
    }

    printf("RESPONSE:\n%s", response);

    int response_content_start = strpos(response, "\r\n\r\n");
    char response_content[BUF_SIZE];
    strcpy(response_content, response + response_content_start + 4);
    //printf("==============\n%s", response_content);

    close(clientfd);
    return 0;
}

/**
 * Function to get request line of HTTP request
 *
 * @param command Command structure containing all command arguments and type
 * @return String representing the request line
 */
char *get_request_line(Command_t *command) {
    char *request_line = NULL;
    switch (command->type) {
        case BOARDS:
            // Request line: GET /boards HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * strlen("GET /boards HTTP/1.1\r\n"));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "GET /boards HTTP/1.1\r\n");
            break;
        case BOARD_ADD:
            // Request line: POST /boards/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("POST /boards/") + strlen(command->name) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "POST /boards/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n");
            break;
        case BOARD_DELETE:
            // Request line: DELETE /boards/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("DELETE /boards/") + strlen(command->name) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "DELETE /boards/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n");
            break;
        case BOARD_LIST:
            // Request line: GET /board/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("GET /board/") + strlen(command->name) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "GET /board/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n");
            break;
        case ITEM_ADD:
            // Request line: POST /board/<name> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("POST /board/") + strlen(command->name) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "POST /board/");
            strcat(request_line, command->name);
            strcat(request_line, " HTTP/1.1\r\n");
            break;
        case ITEM_DELETE:
            // Request line: DELETE /board/<name>/<id> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("DELETE /board/") + strlen(command->name) + 1 + strlen(command->id) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "DELETE /board/");
            strcat(request_line, command->name);
            strcat(request_line, "/");
            strcat(request_line, command->id);
            strcat(request_line, " HTTP/1.1\r\n");
            break;
        case ITEM_UPDATE:
            // Request line: PUT /board/<name>/<id> HTTP/1.1
            request_line = (char *) malloc(sizeof(char) * (strlen("PUT /board/") + strlen(command->name) + 1 + strlen(command->id) + strlen(" HTTP/1.1\r\n")));
            if (request_line == NULL) {
                return NULL;
            }
            strcpy(request_line, "PUT /board/");
            strcat(request_line, command->name);
            strcat(request_line, "/");
            strcat(request_line, command->id);
            strcat(request_line, " HTTP/1.1\r\n");
            break;
        case UNKNOWN:
            return NULL;
    }

    return request_line;
}

/**
 * Function to get the header specifying the host
 *
 * @param destination Where to send HTTP request (host and port)
 * @return String representing the host header
 */
char *get_host_header(Address_t *destination) {
    char port_str[6];
    sprintf(port_str, "%d", destination->port);

    char *host_header = (char *) malloc(sizeof(char) * (strlen("Host: ") + strlen(destination->host) + 1 + strlen(port_str) + strlen("\r\n")));
    if (host_header == NULL) {
        return NULL;
    }

    strcpy(host_header, "Host: ");
    strcat(host_header, destination->host);
    strcat(host_header, ":");
    strcat(host_header, port_str);
    strcat(host_header, "\r\n");

    return host_header;
}

/**
 * Function to get the header specifying the content sent
 *
 * @param command Command structure containing all command arguments and type
 * @return String representing the content header
 */
char *get_content_header(Command_t *command) {
    if (command->type != ITEM_ADD && command->type != ITEM_UPDATE) {
        // No content for these commands
        return NULL;
    }

    // Count length of content
    int length = 0;
    int i = 0;
    while (i < strlen(command->content)) {
        if (command->content[i] == '\\') {
            if (i+1 < strlen(command->content) && command->content[i+1] == 'n') {
                // New line - skip, do not count
                i += 2;
            }
        }
        else {
            length++;
            i++;
        }
    }

    char length_str[5];
    sprintf(length_str, "%d", length);

    char *content_header = (char *) malloc(sizeof(char) * (strlen("Content-Type: text/plain\r\nContent-Length: ") + strlen(length_str) + strlen("\r\n")));
    if (content_header == NULL) {
        return NULL;
    }
    strcpy(content_header, "Content-Type: text/plain\r\nContent-Length: ");
    strcat(content_header, length_str);
    strcat(content_header, "\r\n");

    return content_header;
}

/**
 * Function to get string representing the message body in HTTP request
 *
 * @param command Command structure containing all command arguments and type
 * @return String representing the message body
 */
char *get_message_body(Command_t *command) {
    if (command->type != ITEM_ADD && command->type != ITEM_UPDATE) {
        // No content for these commands
        return NULL;
    }

    // Count length of content
    int length = 0;
    int new_lines = 0;
    int i = 0;
    while (i < strlen(command->content)) {
        if (command->content[i] == '\\') {
            if (i+1 < strlen(command->content) && command->content[i+1] == 'n') {
                // New line - skip, do not count
                new_lines++;
                i += 2;
            }
        }
        else {
            length++;
            i++;
        }
    }

    char *message_body = (char *) malloc(sizeof(char) * (length + new_lines));
    if (message_body == NULL) {
        return NULL;
    }

    i = 0;
    int index = 0;
    while (i < strlen(command->content)) {
        if (command->content[i] == '\\') {
            if (i+1 < strlen(command->content) && command->content[i+1] == 'n') {
                // New line - skip, do not count
                message_body[index++] = '\n';
                i += 2;
            }
        }
        else {
            message_body[index++] = command->content[i];
            i++;
        }
    }

    return message_body;
}

/**
 * Function to get host info
 *
 * @param host Host address
 * @param port Port number (string form)
 * @return Structure representing host address
 */
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

/**
 * Function to establish connection
 *
 * @param info Host info
 * @return Success or failure
 */
int establish_connection(struct addrinfo *info) {
    if (info == NULL) return -1;

    int clientfd;
    for (;info != NULL; info = info->ai_next) {
        if ((clientfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) < 0) {
            fprintf(stderr, "Error establishing connection\n");
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
