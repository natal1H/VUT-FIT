/**
 * @file isaserver.c
 * @author Natalia Holkova (xholko02), FIT
 * @date 18.11.2019
 * @brief Implements server for receiving HTTP requests and sending back responses
 *
 * Project for ISA, task variant - HTTP dashboard
 */

#include "isaserver.h"

int main(int argc, char **argv) {

    int port = -1;

    int ret = parse_arguments(argc, argv, &port);
    printf("Ret: %d\n", ret);
    if (ret != 0) {
        // -h argument or incorrect usage - quit program
        return (ret == 1 ? 0 : 1);
    }

    printf("Port: %d\n", port);

    boards = init_boards();

    // Bind a listener
    int server = bind_listener(get_addr_info(port));
    if (server < 0) {
        fprintf(stderr, "Error! Failed to bind to port.\n");
        return -1;
    }

    if (listen(server, 10) < 0) {
        fprintf(stderr, "Error while trying to listen.\n");
        return -1;
    }

    // Accept incomming requests
    int handler;
    socklen_t size;
    struct sockaddr_storage client;
    while (1) {
        size = sizeof(client);
        handler = accept(server, (struct sockaddr *)&client, &size);
        if (handler < 0) {
            fprintf(stderr, "Error! Failed to accept.\n");
            continue;
        }
        resolve(handler);
        close(handler);
        printf("==============================\n");
    }

    close(server);
    return 0;
}

/**
 * Function to parse command line arguments
 *
 * @param n Number of arguments
 * @param args Arguments
 * @param port Pointer to port number
 * @return 0 - correct usage (port selected), 1 - help, -1 - incorrect usage
 */
int parse_arguments(int n, char **args, int *port) {
    if (n == 2 && strcmp(args[1], "-h") == 0) {
        // -h argument: display usage
        printf("HTTP dashboard server\n");
        printf("Usage: ./isaserver -p <port>\n");
        return 1;
    }
    else if (n == 3 && strcmp(args[1], "-p") == 0) {
        // Attempting to select port - check if correct form
        if (is_integer(args[2])) {
            // Get port as an integer
            int tmp = atoi(args[2]);

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
 * Function to get address info when provided port number
 *
 * @param port Port number
 * @return Address structure
 */
struct addrinfo *get_addr_info(int port) {
    int r;
    struct addrinfo hints, *getaddrinfo_res;

    char port_str[6];
    sprintf(port_str, "%d", port);

    // Setup hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if ((r = getaddrinfo(NULL, port_str, &hints, &getaddrinfo_res))) {
        fprintf(stderr, "Error getting address.\n");
        return NULL;
    }

    return getaddrinfo_res;
}

/**
 * Function to bind socket to listen at port
 *
 * @param info Address structure
 * @return Bound socket or -1 on failure
 */
int bind_listener(struct addrinfo *info) {
    if (info == NULL) return -1;

    int serverfd;
    for (;info != NULL; info = info->ai_next) {
        if ((serverfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) < 0) {
            fprintf(stderr, "Error creating socket.\n");
            continue;
        }

        int opt = 1;
        if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0) {
            fprintf(stderr, "Error setting socket\n");
            return -1;
        }

        if (bind(serverfd, info->ai_addr, info->ai_addrlen) < 0) {
            close(serverfd);
            fprintf(stderr, "Error binding socket.\n");
            continue;
        }

        freeaddrinfo(info);
        return serverfd;
    }

    freeaddrinfo(info);
    return -1;
}

/**
 * Function to resolve received packet
 *
 * @param handler Socket handler
 */
void resolve(int handler) {
    //int size;
    char buf[BUF_SIZE];
    char head[BUF_SIZE];
    char body[BUF_SIZE];

    memset(head,0,BUF_SIZE);

    recv(handler, buf, BUF_SIZE, 0);
    printf("Request: %s\n", buf);

    // Get only request header before \r\n
    int request_header_end = strpos(buf, "\r\n");
    int content_start = strpos(buf, "\r\n\r\n");
    strncpy(head, buf, request_header_end);
    int index = 0;
    for (int i = content_start + 4; i < strlen(buf); i++)
        body[index++] = buf[i];
    body[index] = '\0';
    printf("Content: %s\n", body);

    Command_type type = determine_command(head);
    int err_name = 0;
    char *name = get_command_arg_name(type, head, &err_name);
    printf("Name param: %s\n", name);
    int err_id = 0;
    char *id = get_command_arg_id(type, head, &err_id);
    printf("Id param: %s\n", id);
    int run_err = 0;
    char *content_to_send = run_command(type, name, id, body, &run_err);

    header(handler, 200, content_to_send);

    memset(buf,0,BUF_SIZE);
}

/**
 * Function to create header for response packet and send it
 *
 * @param handler Socket handler
 * @param status Response status to send
 * @param content Content to send in response
 */
void header(int handler, int status, char *content) {
    char header[BUF_SIZE] = {0};
    if (status == 200) {
        sprintf(header, "HTTP/1.1 200 OK\r\n");
    }
    if (status == 201) {
        sprintf(header, "HTTP/1.1 201 OK\r\n");
    }
    else if (status == 403) {
        sprintf(header, "HTTP/1.1 403 Forbidden\r\n");
    }
    else if (status == 404) {
        sprintf(header, "HTTP/1.1 404 Not found\r\n");
    }

    if (content != NULL) {
        int content_len = strlen(content);
        char len_str[5];
        sprintf(len_str, "%d", content_len);
        strcat(header, "Content-Type: text/plain\r\nContent-Length: ");
        strcat(header, len_str);
        strcat(header, "\r\n\r\n");
        strcat(header, content);
    }
    else {
        strcat(header, "\r\n");
    }

    send(handler, header, strlen(header), 0);
}

/**
 * Function to determine type of command received
 *
 * @param command String containing received command
 * @return Type of command
 */
Command_type determine_command(char command[BUF_SIZE]) {
    char *method;
    printf("COMMAND: %s\n", command);

    char tmp[BUF_SIZE];
    strcpy(tmp, command);

    method = strtok(tmp, " ");
    printf("Method: %s\n", method);

    if (strcmp(method, "GET") == 0) {
        // GET method
        printf("GET\n");
        // Could be GET /boards or GET /board/<name>
        if (strlen(command) == strlen("GET /boards HTTP/1.1") && strcmp(command, "GET /boards HTTP/1.1") == 0){
            // GET /boards
            printf("Type: GET /boards\n");
            return BOARDS;
        }
        else if (strncmp(command, "GET /board/", strlen("GET /board/")) == 0) {
            // GET /board/<name>
            printf("Type: GET /board/\n");

            return BOARD_LIST;
        }
        else {
            fprintf(stderr, "Invalid GET request.\n");
            return UNKNOWN;
        }
    }
    else if (strcmp(method, "POST") == 0) {
        // POST method
        printf("POST\n");
        // Could be POST /boards/<name> or POST /board/<name>
        if (strncmp(command, "POST /boards/", strlen("POST /boards/")) == 0) {
            printf("Type: POST /boards/\n");

            return BOARD_ADD;
        }
        else if (strncmp(command, "POST /board/", strlen("POST /board/")) == 0) {
            printf("Type: POST /board/\n");

            return ITEM_ADD;
        }
        else {
            fprintf(stderr, "Invalid POST request.\n");
            return UNKNOWN;
        }
    }
    else if (strcmp(method, "DELETE") == 0) {
        // DELETE method
        printf("DELETE\n");
        // Could be DELETE /boards/<name> or DELETE /board/<name>/<id>
        if (strncmp(command, "DELETE /boards/", strlen("DELETE /boards/")) == 0) {
            printf("Type: DELETE /boards/\n");

            return BOARD_DELETE;
        }
        else if (strncmp(command, "DELETE /board/", strlen("DELETE /board/")) == 0) {
            printf("Type: DELETE /board/\n");

            return ITEM_DELETE;
        }
        else {
            fprintf(stderr, "Invalid DELETE request.\n");
            return UNKNOWN;
        }
    }
    else if (strcmp(method, "PUT") == 0) {
        // POST method
        printf("PUT\n");
        // Should be only PUT /board/<name>/<id>
        if (strncmp(command, "PUT /board/", strlen("PUT /board/")) == 0) {
            printf("Type: PUT /board/\n");

            return ITEM_UPDATE;
        }
        else {
            fprintf(stderr, "Invalid PUT request.\n");
            return UNKNOWN;
        }
    }
    else {
        // Unknown method
        printf("Unknown\n");
        return UNKNOWN;
    }
}

/**
 * Function to get <name> argument from whole command
 *
 * @param type Type of command
 * @param command String containing whole commnad
 * @param err Pointer to integer where to store error status
 * @return String representing <name> argument
 */
char *get_command_arg_name(Command_type type, char *command, int *err) {
    *err = 0;
    if (type == BOARDS) {
        // GET /boards doesn't have <name> arg
        return NULL;
    }
    else if (type == BOARD_ADD) {
        int space_index = get_index(&command[strlen("POST /boards/")], ' ');
        char *name = (char *) malloc(sizeof(char) * (space_index));
        if (name == NULL) {
            fprintf(stderr, "Error while trying to allocate space for name.\n");
            *err = 1;
        }
        strncpy(name, &command[strlen("POST /boards/")], space_index);

        return name;
    }
    else if (type == BOARD_DELETE) {
        int space_index = get_index(&command[strlen("DELETE /boards/")], ' ');
        char *name = (char *) malloc(sizeof(char) * (space_index));
        if (name == NULL) {
            fprintf(stderr, "Error while trying to allocate space for name.\n");
            *err = 1;
        }
        strncpy(name, &command[strlen("DELETE /boards/")], space_index);

        return name;
    }
    else if (type == BOARD_LIST) {
        int space_index = get_index(&command[strlen("GET /board/")], ' ');
        char *name = (char *) malloc(sizeof(char) * (space_index));
        if (name == NULL) {
            fprintf(stderr, "Error while trying to allocate space for name.\n");
            *err = 1;
        }
        strncpy(name, &command[strlen("GET /board/")], space_index);

        return name;
    }
    else if (type == ITEM_ADD) {
        int space_index = get_index(&command[strlen("POST /board/")], ' ');
        char *name = (char *) malloc(sizeof(char) * (space_index));
        if (name == NULL) {
            fprintf(stderr, "Error while trying to allocate space for name.\n");
            *err = 1;
        }
        strncpy(name, &command[strlen("POST /board/")], space_index);

        return name;
    }
    else if (type == ITEM_DELETE) {
        int slash_index = get_index(&command[strlen("DELETE /board/")], '/');
        char *name = (char *) malloc(sizeof(char) * (slash_index));
        if (name == NULL) {
            fprintf(stderr, "Error while trying to allocate space for name.\n");
            *err = 1;
        }
        strncpy(name, &command[strlen("DELETE /board/")], slash_index);

        return name;
    }
    else if (type == ITEM_UPDATE) {
        int slash_index = get_index(&command[strlen("PUT /board/")], '/');
        char *name = (char *) malloc(sizeof(char) * (slash_index));
        if (name == NULL) {
            fprintf(stderr, "Error while trying to allocate space for name.\n");
            *err = 1;
        }
        strncpy(name, &command[strlen("PUT /board/")], slash_index);

        return name;
    }
    else {
        return NULL;
    }
}

/**
 * Function to get <id> argument from whole command
 *
 * @param type Type of command
 * @param command String containing whole commnad
 * @param err Pointer to integer where to store error status
 * @return String representing <id> argument
 */
char *get_command_arg_id(Command_type type, char *command, int *err) {
    *err = 0;
    if (type == ITEM_DELETE) {
        int slash_index = get_index(&command[strlen("DELETE /board/")], '/');
        int space_index = get_index(&command[strlen("DELETE /board/") + slash_index], ' ');
        char *id = (char *) malloc(sizeof(char) * (space_index));
        if (id == NULL) {
            fprintf(stderr, "Error while trying to allocate id for name.\n");
            *err = 1;
        }
        strncpy(id, &command[strlen("DELETE /board/") + slash_index + 1], space_index - 1);

        return id;
    }
    else if (type == ITEM_UPDATE) {
        int slash_index = get_index(&command[strlen("PUT /board/")], '/');
        int space_index = get_index(&command[strlen("PUT /board/") + slash_index], ' ');
        char *id = (char *) malloc(sizeof(char) * (space_index));
        if (id == NULL) {
            fprintf(stderr, "Error while trying to allocate id for name.\n");
            *err = 1;
        }
        strncpy(id, &command[strlen("PUT /board/") + slash_index + 1], space_index - 1);

        return id;
    }
    else {
        return NULL;
    }
}

/**
 * Determine what to do based on type of command received
 *
 * @param type Type of command
 * @param name Name argument
 * @param id Id argument
 * @param content Content argument
 * @param err Pointer to integer where to store error status
 * @return String representing content to send back to client
 */
char *run_command(Command_type type, char *name, char *id, char *content, int *err) {

    char *content_to_send = NULL;

    if (type == BOARDS) {
        content_to_send = list_boards(boards);
    }
    else if (type == BOARD_ADD) {
        Board_t *board = create_board(name);
        add_board(boards, board);
        printf("YAY!\n");
    }
    else if (type == BOARD_DELETE) {
        delete_board(boards, name);
    }
    else if (type == BOARD_LIST) {
        content_to_send = list_board(boards, name);
        printf("ITEMS:\n%s\n", content_to_send);
    }
    else if (type == ITEM_ADD) {
        item_add(boards, name, content);
        printf("Item added.\n");
    }
    else if (type == ITEM_DELETE) {
        item_delete(boards, name, id);
        printf("Item deleted.\n");
    }
    else if (type == ITEM_UPDATE) {
        item_update(boards, name, id, content);
        printf("Item updated\n");
    }

    return content_to_send;
}

/**
 * Initialize Boards structure
 *
 * @return Pointer to initialized structure
 */
Boards_t *init_boards() {
    Boards_t *b = (Boards_t *) malloc(sizeof(Boards_t));
    b->first = NULL;
    return b;
}

/**
 * Add new board to boards
 *
 * @param boards Pointer to boards
 * @param board Pointer to new board to add
 * @return Success or failure
 */
int add_board(Boards_t *boards, Board_t *board) {
    if (boards->first == NULL) {
        // Set board to be first
        boards->first = board;
    }
    else {
        // Find last board
        Board_t *tmp = boards->first;
        while (tmp->next != NULL) {
            if (strcmp(tmp->name, board->name) == 0) {
                fprintf(stderr, "Board with name %s already exists.\n", board->name);
                return 1;
            }
            tmp = tmp->next;
        }
        if (strcmp(tmp->name, board->name) == 0) {
            fprintf(stderr, "Board with name %s already exists.\n", board->name);
            return 1;
        }
        // tmp is the last board
        tmp->next = board;
    }

    return 0;
}

/**
 * Function to create new board
 *
 * @param name Name of the board
 * @return Pointer to new board
 */
Board_t *create_board(char *name) {
    Board_t *board = (Board_t *) malloc(sizeof(Board_t));
    board->name = (char *) malloc(sizeof(char) * strlen(name));
    strcpy(board->name, name);
    board->first = NULL;
    board->next = NULL;

    return board;
}

/**
 * Function to get list of names of all boards
 *
 * @param boards Pointer to structure containing all boards
 * @return String of names of boards
 */
char *list_boards(Boards_t *boards) {
    if (boards->first == NULL)
        return NULL;

    char *content_to_send = (char *) malloc(sizeof(char) * BUF_SIZE); // TODO temp

    Board_t *tmp = boards->first;
    while (tmp != NULL) {
        printf("%s\n", tmp->name);
        strcat(content_to_send, tmp->name);
        strcat(content_to_send, "\n");

        tmp = tmp->next;
    }

    return content_to_send;
}

/**
 * Function to delete board from boards
 *
 * @param boards Pointer to structure containing all boards
 * @param name Name of board to delete
 * @return Success or failure
 */
int delete_board(Boards_t *boards, char *name) {
    if (boards->first == NULL) {
        fprintf(stderr, "No boards\n");
        return 1;
    }

    Board_t *tmp = boards->first;
    Board_t *prev;
    bool found = false;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            found = true;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    if (!found) {
        fprintf(stderr, "No board with name %s\n", name);
        return 2;
    }

    if (tmp == boards->first && tmp->next == NULL) {
        boards->first = NULL;
    }
    else if (tmp == boards->first) {
        boards->first = tmp->next;
    }
    else {
        prev->next = tmp->next;
    }
    // Free the board
    free(tmp->name);
    free(tmp);

    return 0;
}

/**
 * Function to list all items in a board
 *
 * @param boards Pointer to structure containing all boards
 * @param name Name of board where list all items
 * @return String of all items in a board
 */
char *list_board(Boards_t *boards, char *name) {
    if (boards->first == NULL) {
        return NULL;
    }

    Board_t *tmp = boards->first;
    bool found = false;
    while (tmp != NULL) {
        printf("BOARD NAME... %s\n", tmp->name);
        if (strcmp(tmp->name, name) == 0) {
            found = true;
            break;
        }
        tmp = tmp->next;
    }

    if (!found) {
        fprintf(stderr, "No board with name %s\n", name);
        return NULL;
    }

    char *items = (char *) malloc(sizeof(char) * BUF_SIZE); // TODO temp

    int items_num = 0;
    char id[MAX_ITEMS];
    Item_t *tmp_item = tmp->first;
    printf("===========\n");
    while (tmp_item != NULL) {
        printf("%s\n", tmp_item->content);
        sprintf(id, "%d. ", ++items_num);
        strcat(items, id);
        strcat(items, tmp_item->content);
        strcat(items, "\n");

        tmp_item = tmp_item->next;
    }
    printf("===========\n");

    return items;
}

/**
 * Function to add item to a board
 *
 * @param boards Pointer to structure containing all boards
 * @param name Name of the board where to add item
 * @param content Content of new item
 * @return Success or failure
 */
int item_add(Boards_t *boards, char *name, char *content) {
    if (boards->first == NULL) {
        return -1;
    }

    printf("-- p 1\n");

    Board_t *tmp = boards->first;
    bool found = false;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            found = true;
            break;
        }
        tmp = tmp->next;
    }

    printf("-- p 2\n");

    if (!found) {
        fprintf(stderr, "No board with name %s\n", name);
        return -1;
    }

    if (tmp->first == NULL) {
        //printf("-- p 3a\n");
        // Empty board
        tmp->first = (Item_t *) malloc(sizeof(Item_t));
        //printf("...%s\n", tmp->first == NULL ? "NULL" : "NOT NULL");
        //printf("-- p 3ab, %s\n", content);
        tmp->first->content = (char *) malloc(sizeof(char) * strlen(content));
        //printf("-- p 3ac\n");
        strcpy(tmp->first->content, content);
        //printf("-- p 4a\n");
    }
    else {
        //printf("-- p 3b\n");
        // Board not empty
        Item_t *tmp_item = tmp->first;
        while (tmp_item->next != NULL) {
            tmp_item = tmp_item->next;
        }
        //printf("Last item. %s\n", tmp)

        Item_t *new_item = (Item_t *) malloc(sizeof(Item_t));
        new_item->content = (char *) malloc(sizeof(char) * strlen(content));
        strcpy(new_item->content, content);
        tmp_item->next = new_item;
        new_item->next = NULL;
        //printf("-- p 4b\n");
    }

    return 0;
}

/**
 * Function to delete item from a board
 *
 * @param boards Pointer to structure containing all boards
 * @param name Name of the board where to delete item
 * @param id Id of item to delete
 * @return Success or failure
 */
int item_delete(Boards_t *boards, char *name, char *id) {
    int id_num = atoi(id);

    if (boards->first == NULL) {
        fprintf(stderr, "No boards\n");
        return 1;
    }

    Board_t *tmp = boards->first;
    bool found = false;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            found = true;
            break;
        }
        tmp = tmp->next;
    }

    if (!found) {
        fprintf(stderr, "No board with name %s\n", name);
        return 2;
    }

    if (tmp->first == NULL) {
        // Empty board
        fprintf(stderr, "Empty board\n");
        return 2;
    }

    if (id_num == 1) {
        // Delete first
        Item_t *del = tmp->first;
        tmp->first = del->next;

        free(del->content);
        free(del);
    }
    else {
        bool found_item = false;
        int items_num = 0;
        Item_t *tmp_item = tmp->first;
        Item_t *prev;

        while (tmp_item != NULL) {
            items_num++;

            if (items_num == id_num) {
                // This one will be deleted
                found_item = true;
                prev->next = tmp_item->next;
                free(tmp_item->content);
                free(tmp_item);
                break;
            }

            prev = tmp_item;
            tmp_item = tmp_item->next;
        }

        if (!found_item) {
            // Not valid id
            return 3;
        }
    }
    return 0;
}

/**
 * Function to update item content in a board
 *
 * @param boards Pointer to structure containing all boards
 * @param name Name of the board where to update item
 * @param id Id of item to update
 * @param content New content
 * @return Success or failure
 */
int item_update(Boards_t *boards, char *name, char *id, char *content) {
    int id_num = atoi(id);

    if (boards->first == NULL) {
        fprintf(stderr, "No boards\n");
        return 1;
    }

    Board_t *tmp = boards->first;
    bool found = false;
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            found = true;
            break;
        }
        tmp = tmp->next;
    }

    if (!found) {
        fprintf(stderr, "No board with name %s\n", name);
        return 2;
    }

    if (tmp->first == NULL) {
        // Empty board
        fprintf(stderr, "Empty board\n");
        return 2;
    }

    if (id_num == 1) {
        // Update first
        free(tmp->first->content);
        tmp->first->content = (char *) malloc(sizeof(char) * strlen(content));
        strcpy(tmp->first->content, content);
    }
    else {
        bool found_item = false;
        int items_num = 0;
        Item_t *tmp_item = tmp->first;

        while (tmp_item != NULL) {
            items_num++;

            if (items_num == id_num) {
                // This one will be updated
                free(tmp_item->content);
                tmp_item->content = (char *) malloc(sizeof(char) * strlen(content));
                strcpy(tmp_item->content, content);
                break;
            }

            tmp_item = tmp_item->next;
        }

        if (!found_item) {
            // Not valid id
            return 3;
        }
    }
    return 0;
}
