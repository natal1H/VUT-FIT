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