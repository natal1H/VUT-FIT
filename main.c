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

int *check_port_range(char *ports_str, int *ports_len) {
    int *ports_arr = NULL;
    char *res = strchr(ports_str, '-');
    if (res == NULL) {
        // Either format X,Y,Z,..., single port or wrong format

        // Test if is integer (a.k.a single port
        if (is_integer(ports_str)) {
            int port = atoi(ports_str);

            ports_arr = (int *) malloc(sizeof(int) * 1); // single port
            ports_arr[0] = port;
            *ports_len = 1;
        }
        else {
            res = strchr(ports_str, ',');
            if (res != NULL) {
                // Contains at least one ,
                int expected_ports = count_char_occurrences(ports_str, ',') + 1; // Count number of , in string

                ports_arr = (int *) malloc(sizeof(int) * expected_ports);
                *ports_len = expected_ports;

                char tmp_port[5]; tmp_port[0] = '\0';
                char *p_str = ports_str;
                int index = 0, count = 0;
                for (int i = 0; i < strlen(p_str); i++) {
                    if (p_str[i] >= '0' && p_str[i] <= '9') {
                        tmp_port[index++] = p_str[i];
                        tmp_port[index] = '\0';
                    }
                    else if (p_str[i] == ',') {
                        // Convert port number in tmp_port and add to array
                        int port_number = atoi(tmp_port);
                        index = 0;
                        ports_arr[count++] = port_number;
                        tmp_port[0] = '\0';
                    }
                    else {
                        // Error! Invalid character in port range
                        fprintf(stderr, "Error! Invalid character in UDP port range.\n");
                        free(ports_arr); // Free allocated memory
                        return NULL;
                    }
                }
                // Add last one stuck in tmp_port
                int port_number = atoi(tmp_port);
                ports_arr[count++] = port_number;
            }
            else {
                // Error! wrong port range format
                fprintf(stderr, "Error! Wrong format of UDP port range.\n");
                return NULL;
            }
        }


    }
    else {
        // Format X-Y (from X to Y including)
        char *first_part = (char *) malloc(sizeof(char) * (res - ports_str));
        memcpy(first_part, ports_str, res - ports_str);

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

        ports_arr = (int *) malloc(sizeof(int) * expected_ports);
        *ports_len = expected_ports;
        for (int i = 0, j = begin_port; j <= end_port; i++, j++) {
            ports_arr[i] = j;
        }
    }

    return ports_arr;
}

int ip_version(char *src) {
    char buf[16];
    if (inet_pton(AF_INET, src, buf)) {
        return 4;
    } else if (inet_pton(AF_INET6, src, buf)) {
        return 6;
    }
    return -1;
}

// inspired by: http://www.logix.cz/michal/devel/various/getaddrinfo.c.xp
char *lookup_host (const char *host, int *ver) {
    struct addrinfo hints, *res;
    int errcode;
    char addrstr[100];
    char *dest_address = (char *) malloc(sizeof(char) * 100); // TODO
    void *ptr;

    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo (host, NULL, &hints, &res);
    if (errcode != 0)
    {
        perror("getaddrinfo");
        return NULL;
    }

    printf ("Host: %s\n", host);
    while (res)
    {
        inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

        switch (res->ai_family)
        {
            case AF_INET:
                ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
                break;
        }
        inet_ntop (res->ai_family, ptr, addrstr, 100);
        printf ("IPv%d address: %s (%s)\n", res->ai_family == PF_INET6 ? 6 : 4,
                addrstr, res->ai_canonname);

        strcpy(dest_address, addrstr);
        printf("Copying %s\n", addrstr);
        if (res->ai_family != PF_INET6) {// If it has both, use first IPv4
            *ver = 4;
            break;
        }
        else {
            *ver = 6;
        }

        res = res->ai_next;
    }

    return dest_address;
}

// Inspired by: http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
int get_source_ip(char *source_ip, int version, char *interface) {
    int fd;
    struct ifreq ifr;

    if (version == 4) {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        // I want to get an IPv4 IP address
        ifr.ifr_addr.sa_family = AF_INET;
    }
    else {
        fd = socket(AF_INET6, SOCK_DGRAM, 0);
        // I want to get an IPv4 IP address
        ifr.ifr_addr.sa_family = AF_INET6;
    }

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    // display result
    printf("source IP: %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    strcpy(source_ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return 0;
}

int main(int argc, char **argv) {
    Params_t params;
    init_params(&params);

    int *udp_ports = NULL;
    int *tcp_ports = NULL;
    int udp_ports_len = 0, tcp_ports_len = 0;
    char *destination_address = NULL;
    int dest_ip_version;
    char *source_address = NULL;

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
        udp_ports = check_port_range(params.port_range_udp, &udp_ports_len);
        if (udp_ports == NULL)
            return ERR_PARAMS;
    }
    // TCP port range
    if (params.port_range_tcp != NULL) {
        tcp_ports = check_port_range(params.port_range_tcp, &tcp_ports_len);
        if (tcp_ports == NULL)
            return ERR_PARAMS;
    }

    printf("Interface: %s\n", (params.interface == NULL) ? "null" : params.interface);
    printf("IP address: %s\n", (params.domain_or_ip == NULL) ? "null" : params.domain_or_ip);

    // Check address:
    if (params.domain_or_ip != NULL) {
        int version = ip_version(params.domain_or_ip);
        if (version == -1) {
            // Entered domain, not IP address -> convert it
            printf("User entered domain name\n");
            destination_address = lookup_host(params.domain_or_ip, &dest_ip_version);
            if (destination_address == NULL) {
                exit(ERR_PARAMS);
            }
            else {
                printf("Converted destination address: %s, version: %d\n", destination_address, dest_ip_version);
            }
        }
        else if (version == 4) {
            printf("User entered IPv4 address\n");
            destination_address = params.domain_or_ip;
            dest_ip_version = 4;
        }
        else {
            printf("User entered IPv6 address\n");
            destination_address = params.domain_or_ip;
            dest_ip_version = 6;
        }
    }
    else {
        // Default: localhost
        destination_address = lookup_host("localhost", &dest_ip_version);
        if (destination_address == NULL) {
            exit(ERR_PARAMS);
        }
        else {
            printf("Converted destination address: %s, version: %d\n", destination_address, dest_ip_version);
        }
    }

    // TODO: fix if not input interface

    // Get the source IP
    if (dest_ip_version == 4) {
        source_address = (char *) malloc(sizeof(char) * INET_ADDRSTRLEN);
    }
    else {
        source_address = (char *) malloc(sizeof(char) * INET6_ADDRSTRLEN);
    }
    get_source_ip(source_address, dest_ip_version, params.interface);


    if (udp_ports != NULL) {
        printf("UDP ports:\n");
        for (int i = 0; i < udp_ports_len; i++)
            printf("port %d\n", udp_ports[i]);

        // Call function to perform UDP port scan
        // TODO
    }
    if (tcp_ports != NULL) {
        printf("TCP ports:\n");
        for (int i = 0; i < tcp_ports_len; i++)
            printf("port %d\n", tcp_ports[i]);
        // Call function to perform TCP port scan
        //return tcp_IPv4_port_scan(tcp_ports, tcp_ports_len, params.domain_or_ip, params.interface);
    }

    return ERR_OK; // 0
}
