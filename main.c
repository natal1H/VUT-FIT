/**
 * VUT FIT - IPK - project no. 2
 * Main source file responsible for parsing command line arguments and calling appropriate functions.
 *
 * @file main.c
 * @author Natália Holková (xholko02)
 */

#include "main.h"

void init_params(Params_t *params) {
    params->domain_or_ip = NULL;
    params->interface = NULL;
    params->port_range_tcp = NULL;
    params->port_range_udp = NULL;
}

bool is_integer(char *str) {
    for (int i = 0; i < strlen(str); i++)
        if (str[i] < '0' || str[i] > '9')
            return false;

    return true;
}

int count_char_occurrences(char *str, char ch) {
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
        if (str[i] == ch)
            count++;

    return count;
}

int main(int argc, char **argv) {
    Params_t params;
    init_params(&params);

    int *udp_ports = NULL;
    int *tcp_ports = NULL;
    int udp_ports_len = 0, tcp_ports_len = 0;

    // Parse arguments
    int i = 0;
    while (i < argc) {
        printf("ARG: %s\n", argv[i]);
        if (strcmp(argv[i], "-pu") == 0) {
            // Next should be port range
            if (i+1 < argc) {
                i++;
                params.port_range_udp = argv[i];
                printf("Setting udp range\n");
            }
            else {
                fprintf(stderr, "Error! You need to input UDP port range for scanning after -pu.\n");
                exit(ERR_PARAMS);
            }
        }
        else if (strcmp(argv[i], "-pt") == 0) {
            // Next should be port range
            if (i+1 < argc) {
                i++;
                params.port_range_tcp = argv[i];
            }
            else {
                fprintf(stderr, "Error! You need to input TCP port range for scanning after -pt.\n");
                exit(ERR_PARAMS);
            }
        }
        else if (strcmp(argv[i], "-i") == 0) {
            // Next should be interface
            if (i+1 < argc) {
                i++;
                params.interface = argv[i];
            }
            else {
                fprintf(stderr, "Error! You need to input interface after -i.\n");
                exit(ERR_PARAMS);
            }
        }
        else if (i == argc - 1) {
            // Last argument should be domain-name or IP address
            params.domain_or_ip = argv[i];
        }

        i++;
    }

    // Check port range - which format was input
    // UDP port range
    if (params.port_range_udp != NULL) {
        char *res = strchr(params.port_range_udp, '-');
        if (res == NULL) {
            // Either format X,Y,Z,..., single port or wrong format

            // Test if is integer (a.k.a single port
            if (is_integer(params.port_range_udp)) {
                int port = atoi(params.port_range_udp);

                udp_ports = (int *) malloc(sizeof(int) * 1); // single port
                udp_ports[0] = port;
                udp_ports_len = 1;
            }
            else {
                res = strchr(params.port_range_udp, ',');
                if (res != NULL) {
                    // Contains at least one ,
                    int expected_ports = count_char_occurrences(params.port_range_udp, ',') + 1; // Count number of , in string

                    udp_ports = (int *) malloc(sizeof(int) * expected_ports);
                    udp_ports_len = expected_ports;

                    char tmp_port[5]; tmp_port[0] = '\0';
                    char *udp_p_str = params.port_range_udp;
                    int index = 0, count = 0;
                    for (int i = 0; i < strlen(udp_p_str); i++) {
                        if (udp_p_str[i] >= '0' && udp_p_str[i] <= '9') {
                            tmp_port[index++] = udp_p_str[i];
                            tmp_port[index] = '\0';
                        }
                        else if (udp_p_str[i] == ',') {
                            // Convert port number in tmp_port and add to array
                            int port_number = atoi(tmp_port);
                            index = 0;
                            udp_ports[count++] = port_number;
                            tmp_port[0] = '\0';
                        }
                        else {
                            // Error! Invalid character in port range
                            fprintf(stderr, "Error! Invalid character in UDP port range.\n");
                            free(udp_ports); // Free allocated memory
                            exit(ERR_PARAMS);
                        }
                    }
                    // Add last one stuck in tmp_port
                    int port_number = atoi(tmp_port);
                    udp_ports[count++] = port_number;
                }
                else {
                    // Error! wrong port range format
                    fprintf(stderr, "Error! Wrong format of UDP port range.\n");
                    exit(ERR_PARAMS);
                }
            }


        }
        else {
            // Format X-Y (from X to Y including)
            char *first_part = (char *) malloc(sizeof(char) * (res - params.port_range_udp));
            memcpy(first_part, params.port_range_udp, res - params.port_range_udp);

            int begin_port = atoi(first_part);
            int end_port = atoi(res + 1);

            free(first_part);

            // swap if begin is bigger than end
            if (begin_port > end_port) {
                int pom = begin_port;
                begin_port = end_port;
                end_port = pom;
            }

            int expected_ports = end_port - begin_port + 1;

            udp_ports = (int *) malloc(sizeof(int) * expected_ports);
            udp_ports_len = expected_ports;
            for (int i = 0, j = begin_port; j <= end_port; i++, j++) {
                udp_ports[i] = j;
            }
        }
    }

    // TCP port range
    if (params.port_range_tcp != NULL) {
        char *res = strchr(params.port_range_tcp, '-');
        if (res == NULL) {
            // Either format X,Y,Z,..., single port or wrong format

            // Test if is integer (a.k.a single port
            if (is_integer(params.port_range_tcp)) {
                int port = atoi(params.port_range_tcp);

                tcp_ports = (int *) malloc(sizeof(int) * 1); // single port
                tcp_ports[0] = port;
                tcp_ports_len = 1;
            }
            else {
                res = strchr(params.port_range_tcp, ',');
                if (res != NULL) {
                    // Contains at least one ,
                    int expected_ports = count_char_occurrences(params.port_range_tcp, ',') + 1; // Count number of , in string

                    tcp_ports = (int *) malloc(sizeof(int) * expected_ports);
                    tcp_ports_len = expected_ports;

                    char tmp_port[5]; tmp_port[0] = '\0';
                    char *tcp_p_str = params.port_range_tcp;
                    int index = 0, count = 0;
                    for (int i = 0; i < strlen(tcp_p_str); i++) {
                        if (tcp_p_str[i] >= '0' && tcp_p_str[i] <= '9') {
                            tmp_port[index++] = tcp_p_str[i];
                            tmp_port[index] = '\0';
                        }
                        else if (tcp_p_str[i] == ',') {
                            // Convert port number in tmp_port and add to array
                            int port_number = atoi(tmp_port);
                            index = 0;
                            tcp_ports[count++] = port_number;
                            tmp_port[0] = '\0';
                        }
                        else {
                            // Error! Invalid character in port range
                            fprintf(stderr, "Error! Invalid character in TCP port range.\n");
                            free(tcp_ports); // Free allocated memory
                            exit(ERR_PARAMS);
                        }
                    }
                    // Add last one stuck in tmp_port
                    int port_number = atoi(tmp_port);
                    tcp_ports[count++] = port_number;
                }
                else {
                    // Error! wrong port range format
                    fprintf(stderr, "Error! Wrong format of TCP port range.\n");
                    exit(ERR_PARAMS);
                }
            }
        }
        else {
            // Format X-Y (from X to Y including)
            char *first_part = (char *) malloc(sizeof(char) * (res - params.port_range_tcp));
            memcpy(first_part, params.port_range_tcp, res - params.port_range_tcp);

            int begin_port = atoi(first_part);
            int end_port = atoi(res + 1);

            free(first_part);

            // swap if begin is bigger than end
            if (begin_port > end_port) {
                int pom = begin_port;
                begin_port = end_port;
                end_port = pom;
            }

            int expected_ports = end_port - begin_port + 1;

            tcp_ports = (int *) malloc(sizeof(int) * expected_ports);
            tcp_ports_len = expected_ports;
            for (int i = 0, j = begin_port; j <= end_port; i++, j++) {
                tcp_ports[i] = j;
            }
        }
    }

    if (udp_ports != NULL) {
        printf("UDP ports:\n");
        for (int i = 0; i < udp_ports_len; i++)
            printf("port %d\n", udp_ports[i]);

        // Call function to perform UDP port scan
        // TODO
    }
    if (tcp_ports != NULL) {
        // Call function to perform TCP port scan
        return tcp_IPv4_port_scan(tcp_ports, tcp_ports_len, params.domain_or_ip, params.interface);
    }

    return ERR_OK; // 0
}