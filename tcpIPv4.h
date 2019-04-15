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
#include <pcap.h>

#include "errCodes.h"

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

// IP header's structure
struct ipheader {
    unsigned char      iph_ihl:5, /* Little-endian */
            iph_ver:4;
    unsigned char      iph_tos;
    unsigned short int iph_len;
    unsigned short int iph_ident;
    unsigned char      iph_flags;
    unsigned short int iph_offset;
    unsigned char      iph_ttl;
    unsigned char      iph_protocol;
    unsigned short int iph_chksum;
    unsigned int       iph_sourceip;
    unsigned int       iph_destip;
};

/* Structure of a TCP header */
struct tcpheader {
    unsigned short int tcph_srcport;
    unsigned short int tcph_destport;
    unsigned int       tcph_seqnum;
    unsigned int       tcph_acknum;
    unsigned char      tcph_reserved:4, tcph_offset:4;
    // unsigned char tcph_flags;
    unsigned int
            tcp_res1:4,      /*little-endian*/
            tcph_hlen:4,     /*length of tcp header in 32-bit words*/
            tcph_fin:1,      /*Finish flag "fin"*/
            tcph_syn:1,       /*Synchronize sequence numbers to start a connection*/
            tcph_rst:1,      /*Reset flag */
            tcph_psh:1,      /*Push, sends data to the application*/
            tcph_ack:1,      /*acknowledge*/
            tcph_urg:1,      /*urgent pointer*/
            tcph_res2:2;
    unsigned short int tcph_win;
    unsigned short int tcph_chksum;
    unsigned short int tcph_urgptr;
};

/**
 *
 * @param handle
 * @param tcp_ports
 * @param num_ports
 * @param dest_address
 * @param source_address
 * @param interface
 * @param ip
 * @return
 */
int tcp_IPv4_port_scan(pcap_t *handle, int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip);

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