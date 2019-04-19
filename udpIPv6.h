#ifndef UDP_IPV6_H
#define UDP_IPV6_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()

#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_UDP, INET6_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <netinet/ip6.h>      // struct ip6_hdr
#include <netinet/udp.h>      // struct udphdr
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
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length
#define UDP_HDRLEN  8  // UDP header length, excludes data

/**
 * Build IPv6 UDP pseudo-header and call checksum function (Section 8.1 of RFC 2460). * @param iphdr IPv6 header
 * @param udphdr UDP header
 * @param payload Payload
 * @param payloadlen Length of payload
 * @return Checksum
 */
uint16_t udp6_checksum(struct ip6_hdr iphdr, struct udphdr udphdr, uint8_t *payload, int payloadlen);

/**
 * Perform UDP port scan (IPv6)
 * @param udp_ports Array containing ports to scan
 * @param num_ports Number of ports to scan
 * @param dest_address Destination IP address
 * @param source_address Source IP address
 * @param selected_interface Name of selected interface
 * @param ip
 */
int udp_IPv6_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *selected_interface, bpf_u_int32 ip);

/**
 * Grab filtered UDP packet
 * @param args Passed arguments
 * @param pkthdr Packet header
 * @param packet Packet payload
 */
void grab_udpIPv6_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

#endif