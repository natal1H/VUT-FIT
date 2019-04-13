#ifndef TCP_IPV4_H
#define TCP_IPV4_H

#define __USE_BSD	// use bsd'ish ip header
#include <sys/socket.h>	// these headers are for a Linux system, but
#include <netinet/in.h>	// the names on other systems are easy to guess..
#include <netinet/ip.h>
#define __FAVOR_BSD	// use bsd'ish tcp header
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>


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
 * @param tcp_ports
 * @param num_ports
 * @param address
 * @param interface
 * @return
 */
int tcp_IPv4_port_scan(int *tcp_ports, int num_ports, char *address, char *interface);

/**
 *
 * @param ptr
 * @param nbytes
 * @return
 */
unsigned short csum(unsigned short *ptr,int nbytes);

void fill_IP_header(struct iphdr *iph, struct sockaddr_in sin, char *data);

/**
 *
 * @param tcph
 * @param dest_port
 */
void fill_TCP_header(struct tcphdr *tcph, int dest_port);

/**
 *
 * @param psh
 * @param sin
 * @param data
 */
void fill_pseudo_header(struct pseudo_header *psh, struct sockaddr_in sin, char *data);

#endif