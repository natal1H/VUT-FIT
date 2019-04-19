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
#include <ifaddrs.h>
#include <errno.h>

#include "tcpIPv4.h"
#include "udpIPv4.h"
#include "tcpIPv6.h"
#include "udpIPv6.h"

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
 * Check port range entered as string and convert it to array of int
 * @param ports_str Port range in string form
 * @param ports_len Port range as array of numbers
 * @return Success or failure
 */
int *check_port_range(char *ports_str, int *ports_len);

/**
 * Lookup host ip address by entered domain
 * @param host Host domain
 * @param ver What version of IP adress was return
 * @return IP address in string form or NULL on failure
 */
char *lookup_host(const char *host, int *ver);

/**
 * Get source IP with selected interface
 * @param source_ip Pointer to where store IP address
 * @param interface Name of interface
 * @param version Preferred version of IP address
 * @return Success or failure
 */
int get_source_ip(char *source_ip, char *interface, int version);

/**
 * Print out how to call program
 */
void display_usage();

/**
 * Print out help message
 */
void display_help();

#endif
