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
#include <stdbool.h>

#include "errCodes.h"
#include "tcpIPv4.h"
#include "udpIPv4.h"

typedef struct {
    char *port_range_tcp;
    char *port_range_udp;
    char *interface;
    char *domain_or_ip;
} Params_t;

/**
 * 96 bit (12 bytes) pseudo header needed for tcp header checksum calculation
 */
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

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
