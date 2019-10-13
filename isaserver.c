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
    }

    close(server);
    return 0;
}

/**
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

void resolve(int handler) {
    //int size;
    char buf[BUF_SIZE];
    char command[BUF_SIZE];
    memset(command,0,BUF_SIZE);

    recv(handler, buf, BUF_SIZE, 0);
    //printf("Request: %s\n", buf);

    // Get only request header before \r\n
    int request_header_end = strpos(buf, "\r\n");
    strncpy(command, buf, request_header_end);

    determine_command(command);

    header(handler, 200);

    memset(buf,0,BUF_SIZE);
}

void header(int handler, int status) {
    char header[BUF_SIZE] = {0};
    if (status == 200) {
        sprintf(header, "HTTP/1.1 200 OK\r\n\r\n");
    }
    else if (status == 403) {
        sprintf(header, "HTTP/1.1 403 Forbidden\r\n\r\n");
    }
    else if (status == 404) {
        sprintf(header, "HTTP/1.1 404 Not found\r\n\r\n");
    }
    send(handler, header, strlen(header), 0);
}

int get_index(char *str, char c) {
    char *tmp = str;
    int index = 0;
    while (*tmp++ != c && index < strlen(str)) index++;
    return (index == strlen(str) ? -1 : index);
}

int strpos(char *hay, char *needle) {
    char haystack[strlen(hay)];
    strncpy(haystack, hay, strlen(hay));
    char *p = strstr(haystack, needle);
    if (p)
        return p - haystack;
    return -1;
}

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
        else {
            // Could be GET /board/<name> or bad request -> get part after GET
            printf("Else\n");
        }

        // Get part after method
    }
    else if (strcmp(method, "POST") == 0) {
        // POST method
        printf("POST\n");
        // Could be POST /boards/<name> or POST /board/<name>
    }
    else if (strcmp(method, "DELETE") == 0) {
        // DELETE method
        printf("DELETE\n");
        // Could be DELETE /boards/<name> or DELETE /board/<name>/<id>
    }
    else if (strcmp(method, "PUT") == 0) {
        // POST method
        printf("PUT\n");
        // Should be only PUT /board/<name>/<id>
    }
    else {
        // Unknown method
        printf("Unknown\n");
        return UNKNOWN;
    }

    return UNKNOWN;
}