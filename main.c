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
    if (inet_pton(AF_INET, src, buf))
        return 4;
    else if (inet_pton(AF_INET6, src, buf))
        return 6;
    else
        return -1;
}

/***************************************************************************************
*    Title: getaddrinfo.c - Simple example of using getaddrinfo(3) function.
*    Author: Michal Ludvig <michal@logix.cz> (c) 2002, 2003
*            http://www.logix.cz/michal/devel/
*    Availability: http://www.logix.cz/michal/devel/various/getaddrinfo.c.xp
***************************************************************************************/
char *lookup_host (const char *host, int *ver) {
    struct addrinfo hints, *res;
    int errcode;
    char addrstr[100];
    char *dest_address = (char *) malloc(sizeof(char) * 100);
    void *ptr;

    memset(&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo(host, NULL, &hints, &res);
    if (errcode != 0) {
        fprintf(stderr, "Error! Couldn't get IP address from domain.\n");
        return NULL;
    }

    while (res) {
        inet_ntop(res->ai_family, res->ai_addr->sa_data, addrstr, 100);

        switch (res->ai_family) {
            case AF_INET:
                ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
                break;
        }
        inet_ntop (res->ai_family, ptr, addrstr, 100);
        //printf("Dest IP: %s\n", addrstr);
        strcpy(dest_address, addrstr);

        if (res->ai_family != PF_INET6) {// If it has both, use first IPv4
            *ver = 4;
            break;
        }
        else {
            *ver = 6;
        }

        /*
        if (res->ai_family != PF_INET) {// If it has both, use first IPv6
            *ver = 6;
            break;
        }
        else
            *ver = 4;
        */

        res = res->ai_next;
    }

    return dest_address;
}

/***************************************************************************************
*    Title: how to get IPV6 interface address using getifaddr() function
*    Author: dbush
*            https://stackoverflow.com/users/1687119/dbush
*    Availability: https://stackoverflow.com/questions/33125710/how-to-get-ipv6-interface-address-using-getifaddr-function
***************************************************************************************/
int get_source_ip(char *source_ip, char *interface, int version) {
    struct ifaddrs *ifa, *ifa_tmp;
    char addr[50];

    if (getifaddrs(&ifa) == -1) {
        perror("getifaddrs failed");
        exit(-1);
    }

    bool found = false;

    ifa_tmp = ifa;
    while (ifa_tmp) {
        if ((ifa_tmp->ifa_addr) && ((ifa_tmp->ifa_addr->sa_family == AF_INET) || (ifa_tmp->ifa_addr->sa_family == AF_INET6))) {
            if (version == 6) {
                if (ifa_tmp->ifa_addr->sa_family == AF_INET6 &&
                    (strcmp(ifa_tmp->ifa_name, interface) == 0)) { // AF_INET6
                    // create IPv6 string
                    struct sockaddr_in6 *in6 = (struct sockaddr_in6 *) ifa_tmp->ifa_addr;
                    inet_ntop(AF_INET6, &in6->sin6_addr, addr, sizeof(addr));

                    strcpy(source_ip, addr);
                    found = true;

                    break;
                }
            } else if (version == 4) {
                if (ifa_tmp->ifa_addr->sa_family == AF_INET &&
                    (strcmp(ifa_tmp->ifa_name, interface) == 0)) { // AF_INET
                    struct sockaddr_in *in = (struct sockaddr_in *) ifa_tmp->ifa_addr;
                    inet_ntop(AF_INET, &in->sin_addr, addr, sizeof(addr));

                    strcpy(source_ip, addr);
                    found = true;

                    break;
                }
            }
        }
        ifa_tmp = ifa_tmp->ifa_next;
    }
    return (found) ? 0 : 1;
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
    bool domain_entered = false;

    if (argc == 1) {
        // No argment
        display_usage();
        return 0;
    }
    else if (argc == 2 && (strcmp(argv[1], "--help") == 0)) {
        // Display help
        display_help();
        return 0;
    }

    // Parse arguments
    int i = 0;
    while (i < argc) {
        if (strcmp(argv[i], "-pu") == 0) {
            // Next should be port range
            if (i+1 < argc) {
                i++;
                params.port_range_udp = argv[i];
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
            return 1;
    }
    // TCP port range
    if (params.port_range_tcp != NULL) {
        tcp_ports = check_port_range(params.port_range_tcp, &tcp_ports_len);
        if (tcp_ports == NULL)
            return 1;
    }

    // Check address:
    if (params.domain_or_ip != NULL) {
        int version = ip_version(params.domain_or_ip);
        if (version == -1) {
            // Entered domain, not IP address -> convert it
            destination_address = lookup_host(params.domain_or_ip, &dest_ip_version);
            if (destination_address == NULL) {
                exit(ERR_PARAMS);
            }
            domain_entered = true;
        }
        else if (version == 4) {
            destination_address = params.domain_or_ip;
            dest_ip_version = 4;
        }
        else {
            destination_address = params.domain_or_ip;
            dest_ip_version = 6;
        }
    }
    else {
        // Domain or IP address not provided
        fprintf(stderr, "Error! Domain or IP address is compulsory argument\n");
        return 1;
    }

    // TODO - if localhost and not -i -> choose lo
    char *interface;
    char error_buffer[PCAP_ERRBUF_SIZE];
    if (params.interface != NULL) {
        interface = params.interface;
    }
    else {
        interface = pcap_lookupdev(error_buffer);
    }

    // Get the source IP
    if (dest_ip_version == 4)
        source_address = (char *) malloc(sizeof(char) * INET_ADDRSTRLEN);
    else
        source_address = (char *) malloc(sizeof(char) * INET6_ADDRSTRLEN);
    get_source_ip(source_address, interface, dest_ip_version);

    bpf_u_int32 subnet_mask, ip;

    if (pcap_lookupnet(interface, &ip, &subnet_mask, error_buffer) == -1) {
        fprintf(stderr, "Error! Couldn't get information for device: %s\n", interface);
        ip = 0;
        subnet_mask = 0;
    }
    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, error_buffer);
    if (handle == NULL) {
        fprintf(stderr, "Error! Couldn't open %s - %s\n", interface, error_buffer);
        return 1;
    }

    printf("Source IP: %s\n", source_address); // TODO - remove

    if (domain_entered)
        printf("Interesting ports on %s (%s):\n", params.domain_or_ip, destination_address);
    else
        printf("Interesting ports on %s:\n", destination_address);

    printf("PORT\t STATE\n");

    if (dest_ip_version == 4) {
        if (udp_ports != NULL) {
            // Call function to perform UDP port scan
            udp_IPv4_port_scan(udp_ports, udp_ports_len, destination_address, source_address, interface, ip);
        }
        if (tcp_ports != NULL) {
            // Call function to perform TCP port scan
            tcp_IPv4_port_scan(tcp_ports, tcp_ports_len, destination_address, source_address, interface, ip);
        }
    }
    else {
        if (udp_ports != NULL) {
            // Call function to perform UDP port scan
            udp_IPv6_port_scan(udp_ports, udp_ports_len, destination_address, source_address, interface, ip);
        }
        if (tcp_ports != NULL) {
            // Call function to perform TCP port scan
            tcp_IPv6_port_scan(tcp_ports, tcp_ports_len, destination_address, source_address, interface, ip);
        }
    }

    pcap_close(handle);

    return 0;
}

void display_usage() {
    printf("Program usage:\n");
    printf("./ipk-scan {-i <interface>} -pu <port-ranges> -pt <port-ranges> [<domain-name> | <IP-address>]\n");
}

void display_help() {
    printf("Application for scanning ports on selected network machine.\n");
    printf("Usage\n");
    printf("\t./ipk-scan {-i <interface>} -pu <port-ranges> -pt <port-ranges> [<domain-name> | <IP-address>]\n");
    printf("where: -pt performs TCP scan\n");
    printf("       -pu performs UDP scan\n");
    printf("       parameters with {} are optional\n");
}