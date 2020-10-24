#ifndef TCP_IPV6_H
#define TCP_IPV6_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()

#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_TCP, INET6_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <netinet/ip6.h>      // struct ip6_hdr
#define __FAVOR_BSD           // Use BSD format of tcp header
#include <netinet/tcp.h>      // struct tcphdr
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <pcap.h>

#include "main.h"
#include "commonIPv6.h"

// Define some constants.
#define ETHER_ADDR_LEN	6
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length
#define TCP_HDRLEN 20  // TCP header length, excludes options data

/**
 * Build IPv6 TCP pseudo-header and call checksum function (Section 8.1 of RFC 2460).
 * @param iphdr
 * @param tcphdr
 * @return
 */
uint16_t tcp6_checksum (struct ip6_hdr iphdr, struct tcphdr tcphdr);

/**
 * Perform TCP port scan (IPv6)
 * @param tcp_ports Array containing ports to scan
 * @param num_ports Number of ports to scan
 * @param dest_address Destination IP address
 * @param source_address Source IP address
 * @param interface Name of interface
 * @param ip
 * @return Success or failure
 */
int tcp_IPv6_port_scan(int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip);

/**
 * Determine if port is open
 * @param th_flags TCP flags
 * @return True if is open
 */
bool is_open_port_tcpIPv6(u_char th_flags);

/**
 * Determine if port is closed
 * @param th_flags TCP flags
 * @return True if is closed
 */
bool is_closed_port_tcpIPv6(u_char th_flags);

/**
 * Grab filtered TCP packet
 * @param args Passed arguments
 * @param pkthdr Packet header
 * @param packet Packet payload
 */
void grab_packet_tcpIPv6(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

/**
 * Get filter expression based on destination port
 * @param port Port that is being scanned
 * @param filter_expr Where to store filter expression
 */
void get_filter_expr_tcpIPv6(int port, char *filter_expr);

#endif