#ifndef UDP_IPV4_H
#define UDP_IPV4_H

#include <stdio.h> //for printf
#include <string.h> //memset
#include <sys/socket.h>    //for socket ofcourse
#include <stdlib.h> //for exit(0);
#include <errno.h> //For errno - the error number
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <arpa/inet.h>

#include "main.h" // Because of pcap_t handle
#include "commonIPv4.h"

int udp_IPv4_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip);

void grab_udp_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

void fill_UDP_header(struct udphdr *udph, int dest_port, char *data);

#endif