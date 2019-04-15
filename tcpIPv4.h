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
#include <stdbool.h>
#include <signal.h>

#include "errCodes.h"
#include "main.h" // because of pcap_t *handle

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

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6
#define SIZE_ETHERNET 14

/* Ethernet header */
struct sniff_ethernet {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
    u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char ip_vhl;		/* version << 4 | header length >> 2 */
    u_char ip_tos;		/* type of service */
    u_short ip_len;		/* total length */
    u_short ip_id;		/* identification */
    u_short ip_off;		/* fragment offset field */
#define IP_RF 0x8000		/* reserved fragment flag */
#define IP_DF 0x4000		/* dont fragment flag */
#define IP_MF 0x2000		/* more fragments flag */
#define IP_OFFMASK 0x1fff	/* mask for fragmenting bits */
    u_char ip_ttl;		/* time to live */
    u_char ip_p;		/* protocol */
    u_short ip_sum;		/* checksum */
    struct in_addr ip_src,ip_dst; /* source and dest address */
};
#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)		(((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;	/* source port */
    u_short th_dport;	/* destination port */
    tcp_seq th_seq;		/* sequence number */
    tcp_seq th_ack;		/* acknowledgement number */
    u_char th_offx2;	/* data offset, rsvd */
#define TH_OFF(th)	(((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;		/* window */
    u_short th_sum;		/* checksum */
    u_short th_urp;		/* urgent pointer */
};

#define PCAP_TIMEOUT 2

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
int tcp_IPv4_port_scan(int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip);

void get_filter_expr_tcpIPv4(int port, char *filter_expr);

/**
 *
 * @param th_flags
 * @return
 */
bool is_open_port(u_char th_flags);

/**
 *
 * @param th_flags
 * @return
 */
bool is_closed_port(u_char th_flags);

/**
 *
 * @param ptr
 * @param nbytes
 * @return
 */
unsigned short csum(unsigned short *ptr,int nbytes);

void fill_IP_header(struct iphdr *iph, struct sockaddr_in sin, char *data, char *source_ip);

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
void fill_pseudo_header(struct pseudo_header *psh, struct sockaddr_in sin, char *data, char *source_ip);

void grab_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

void alarm_handler(int sig);

#endif