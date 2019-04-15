#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pcap.h>

#include "errCodes.h"
#include "tcpIPv4.h"

typedef struct {
    char *port_range_tcp;
    char *port_range_udp;
    char *interface;
    char *domain_or_ip;
} Params_t;

pcap_t *handle;

/**
 * Initialize Params_t structure by setting all attributes to NULL
 * @param params Params_t Structure to be initialized
 */
void init_params(Params_t *params);

/**
 * Check if string represents integer
 * @param str String to check
 * @return Is representing integer?
 */
bool is_integer(char *str);

/**
 * Counts number of occurrences of character in string
 * @param str String where to count character occurrences
 * @param ch Character to find
 * @return Number of occurrences
 */
int count_char_occurrences(char *str, char ch);

/**
 *
 * @param ports_str
 * @param ports_len
 * @return
 */
int *check_port_range(char *ports_str, int *ports_len);

/**
 *
 * @param host
 * @param ver
 * @return
 */
char *lookup_host (const char *host, int *ver);

void display_usage();

void display_help();

#endif
