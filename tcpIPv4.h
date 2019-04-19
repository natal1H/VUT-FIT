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

#include "main.h" // because of pcap_t *handle
#include "commonIPv4.h" // Functions common for UDP and TCP scan with IPv4 address

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

#define PCAP_TIMEOUT 2 // pcap_loop timeout

/**
 * Perform TCP port scan on selected ports (version with IPv4 address)
 * @param tcp_ports Array with numbers of ports
 * @param num_ports Number of ports to scan
 * @param dest_address IPv4 destination address
 * @param source_address IPv4 source address
 * @param interface Name of interface
 * @param ip
 * @return Success or failure
 */
int tcp_IPv4_port_scan(int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip);

/**
 * Get filter expression
 * @param port Number of port which is currently being scanned
 * @param filter_expr Where to store filter expression
 */
void get_filter_expr_tcpIPv4(int port, char *filter_expr);

/**
 * Function to analyze TCP flags and decide if port is open
 * @param th_flags TCP flags
 * @return True if port is open
 */
bool is_open_port(u_char th_flags);

/**
 * Function to analyze TCP flags and decide if port is closed
 * @param th_flags TCP flags
 * @return True if port is closed
 */
bool is_closed_port(u_char th_flags);

/**
 * Fill out TCP header
 * @param tcph Pointer to TCP header
 * @param dest_port Destination port
 */
void fill_TCP_header(struct tcphdr *tcph, int dest_port);

/**
 * Grab filtered TCP packet
 * @param args Passed arguments
 * @param pkthdr Packet header
 * @param packet Packet content
 */
void grab_tcp_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

/**
 * Alarm program when time set for pcap_loop timeout passed
 * @param sig Signal signaling timeout
 */
void alarm_handler(int sig);

#endif