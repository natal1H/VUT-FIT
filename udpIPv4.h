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
#include "errCodes.h"

struct pseudo_header_udpIPv4 {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

unsigned short csum_udpIPv4(unsigned short *ptr,int nbytes);

int udp_IPv4_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip);

void grab_udp_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet);

#endif