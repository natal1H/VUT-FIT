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
//#include <net/ethernet.h>
#include <pcap.h>

#include <errno.h>            // errno, perror()

#include "main.h"
#include "tcpIPv4.h"

// Define some constants.
#define ETH_HDRLEN 14  // Ethernet header length // TODO
#define IP6_HDRLEN 40  // IPv6 header length
#define UDP_HDRLEN  8  // UDP header length, excludes data

// Function prototypes
uint16_t checksum2 (uint16_t *, int);
uint16_t udp6_checksum2 (struct ip6_hdr, struct udphdr, uint8_t *, int);
char *allocate_strmem2 (int);
uint8_t *allocate_ustrmem2 (int);

int udp_IPv6_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *selected_interface, bpf_u_int32 ip);
void grab_udpIPv6_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

#endif