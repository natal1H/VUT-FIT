#ifndef COMMON_IPV4_H
#define COMMON_IPV4_H

#define SRC_PORT 42

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>	// these headers are for a Linux system, but
#include <stdlib.h> //for exit(0);
#include <netinet/ip.h>
#include <stdint.h>
#include <string.h>


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

/**
 *
 * @param ptr
 * @param nbytes
 * @return
 */
unsigned short csum(unsigned short *ptr,int nbytes);

void fill_pseudo_header(struct pseudo_header *psh, struct sockaddr_in sin, char *data, char *source_ip,  uint8_t protocol, size_t size_tcp_udp);

void fill_IP_header(struct iphdr *iph, struct sockaddr_in sin, char *data, char *source_ip, u_int8_t protocol, size_t size_tcp_udp);


#endif