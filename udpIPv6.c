#include "udpIPv6.h"

/***************************************************************************************
 *    Disclaimer: Creating UDP packets was inspired by tutorial:
 *
 *    Title: C Language Examples of IPv4 and IPv6 Raw Sockets for Linux
 *    Availability: http://www.pdbuchan.com/rawsock/rawsock.html
 *                  www.pdbuchan.com/rawsock/udp6_ll.c
 ***************************************************************************************/

/*
 * Perform UDP port scan (IPv6)
 */
int udp_IPv6_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *selected_interface, bpf_u_int32 ip) {
    int i, status, datalen, frame_length, sd, bytes;
    char *interface, *target, *src_ip, *dst_ip;
    struct ip6_hdr iphdr;
    struct udphdr udphdr;
    uint8_t *data, *src_mac, *dst_mac, *ether_frame;
    struct addrinfo hints, *res;
    struct sockaddr_in6 *ipv6;
    struct sockaddr_ll device;
    struct ifreq ifr;
    void *tmp;

    // Allocate memory for various arrays.
    src_mac = allocate_ustrmem(6);
    dst_mac = allocate_ustrmem(6);
    data = allocate_ustrmem(IP_MAXPACKET);
    ether_frame = allocate_ustrmem(IP_MAXPACKET);
    interface = allocate_strmem(40);
    target = allocate_strmem(INET6_ADDRSTRLEN);
    src_ip = allocate_strmem(INET6_ADDRSTRLEN);
    dst_ip = allocate_strmem(INET6_ADDRSTRLEN);

    // Interface to send packet through.
    strcpy (interface, selected_interface);

    // Submit request for a socket descriptor to look up interface.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
        perror ("socket() failed to get socket descriptor for using ioctl() ");
        exit (EXIT_FAILURE);
    }

    // Use ioctl() to look up interface name and get its MAC address.
    memset (&ifr, 0, sizeof (ifr));
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
    if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
        perror ("ioctl() failed to get source MAC address ");
        return (EXIT_FAILURE);
    }
    close (sd);

    // Find interface index from interface name and store index in
    // struct sockaddr_ll device, which will be used as an argument of sendto().
    memset (&device, 0, sizeof (device));
    if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
    }


    // Set destination MAC address: you need to fill these out
    dst_mac[0] = 0xff;
    dst_mac[1] = 0xff;
    dst_mac[2] = 0xff;
    dst_mac[3] = 0xff;
    dst_mac[4] = 0xff;
    dst_mac[5] = 0xff;

    // Source IPv6 address: you need to fill this out
    strcpy (src_ip, source_address);

    // Destination URL or IPv6 address: you need to fill this out
    strcpy (target, dest_address);

    // Fill out hints for getaddrinfo().
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = hints.ai_flags | AI_CANONNAME;

    // Resolve target using getaddrinfo().
    if ((status = getaddrinfo (target, NULL, &hints, &res)) != 0) {
        fprintf (stderr, "getaddrinfo() failed: %s\n", gai_strerror (status));
        exit (EXIT_FAILURE);
    }
    ipv6 = (struct sockaddr_in6 *) res->ai_addr;
    tmp = &(ipv6->sin6_addr);
    if (inet_ntop (AF_INET6, tmp, dst_ip, INET6_ADDRSTRLEN) == NULL) {
        status = errno;
        fprintf (stderr, "inet_ntop() failed.\nError message: %s", strerror (status));
        exit (EXIT_FAILURE);
    }
    freeaddrinfo (res);

    // Fill out sockaddr_ll.
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, src_mac, 6 * sizeof (uint8_t));
    device.sll_halen = 6;

    // UDP data
    datalen = 4;
    data[0] = 'T';
    data[1] = 'e';
    data[2] = 's';
    data[3] = 't';

    // IPv6 header

    // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    iphdr.ip6_flow = htonl ((6 << 28) | (0 << 20) | 0);

    // Payload length (16 bits): UDP header + UDP data
    iphdr.ip6_plen = htons (UDP_HDRLEN + datalen);

    // Next header (8 bits): 17 for UDP
    iphdr.ip6_nxt = IPPROTO_UDP;

    // Hop limit (8 bits): default to maximum value
    iphdr.ip6_hops = 255;

    // Source IPv6 address (128 bits)
    if ((status = inet_pton (AF_INET6, src_ip, &(iphdr.ip6_src))) != 1) {
        fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));
        exit (EXIT_FAILURE);
    }

    // Destination IPv6 address (128 bits)
    if ((status = inet_pton (AF_INET6, dst_ip, &(iphdr.ip6_dst))) != 1) {
        fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));
        exit (EXIT_FAILURE);
    }

    // Fill out ethernet frame header.

    // Ethernet frame length = ethernet header (MAC + MAC + ethernet type) + ethernet data (IP header + UDP header + UDP data)
    frame_length = 6 + 6 + 2 + IP6_HDRLEN + UDP_HDRLEN + datalen;

    // Destination and Source MAC addresses
    memcpy (ether_frame, dst_mac, 6 * sizeof (uint8_t));
    memcpy (ether_frame + 6, src_mac, 6 * sizeof (uint8_t));

    // Next is ethernet type code (ETH_P_IPV6 for IPv6).
    ether_frame[12] = ETH_P_IPV6 / 256;
    ether_frame[13] = ETH_P_IPV6 % 256;

    // Next is ethernet frame data (IPv6 header + UDP header + UDP data).

    // IPv6 header
    memcpy (ether_frame + ETH_HDRLEN, &iphdr, IP6_HDRLEN * sizeof (uint8_t));

    // Submit request for a raw socket descriptor.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
        perror ("socket() failed ");
        exit (EXIT_FAILURE);
    }

    char filter_expr[] = "icmp[0] = 3";
    struct bpf_program filter;
    if (pcap_compile(handle, &filter, filter_expr, 0, ip) == -1) {
        printf("Bad filter - %s\n", pcap_geterr(handle));
        return 1;
    }
    if (pcap_setfilter(handle, &filter) == -1) {
        printf("Error setting filter - %s\n", pcap_geterr(handle));
        return 1;
    }

    for (int curr_port = 0; curr_port < num_ports; curr_port++) {
        int dest_port = udp_ports[curr_port];

        // UDP header
        udphdr.source = htons (42); // Source port number (16 bits): pick a number
        udphdr.dest = htons (dest_port); // Destination port number (16 bits): pick a number
        udphdr.len = htons (UDP_HDRLEN + datalen); // Length of UDP datagram (16 bits): UDP header + UDP data
        udphdr.check = udp6_checksum(iphdr, udphdr, data, datalen); // UDP checksum (16 bits)
        memcpy (ether_frame + ETH_HDRLEN + IP6_HDRLEN, &udphdr, UDP_HDRLEN * sizeof (uint8_t)); // Copy UDP header
        memcpy (ether_frame + ETH_HDRLEN + IP6_HDRLEN + UDP_HDRLEN, data, datalen * sizeof (uint8_t)); // Copy UDP data

        // Send ethernet frame to socket.
        if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
            perror ("sendto() failed");
            exit (EXIT_FAILURE);
        }

        // Grab packet
        int *port_ptr = &dest_port;
        alarm(PCAP_TIMEOUT);
        signal(SIGALRM, alarm_handler_IPv6);

        int ret = pcap_loop(handle, 1, grab_udpIPv6_packet, (u_char *) port_ptr);
        if (ret == -1) {
            fprintf(stderr, "Error! An error occurred in loop\n"); // No need to exit whole program
        }
        else if (ret == -2) { // Breakloop
            // Port open or filtered
            printf("%d/udp\t open\n", dest_port);
        }
    }

    // Close socket descriptor.
    close (sd);

    // Free allocated memory.
    free(src_mac);
    free(dst_mac);
    free(data);
    free(ether_frame);
    free(interface);
    free(target);
    free(src_ip);
    free(dst_ip);

    return 0;
}

/*
 * Grab filtered UDP packet
 */
void grab_udpIPv6_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet) {
    int *checked_port = (int *) args;
    printf("%d/udp\t closed\n", *checked_port);
}

/*
 * Build IPv6 UDP pseudo-header and call checksum function (Section 8.1 of RFC 2460).
 */
uint16_t udp6_checksum(struct ip6_hdr iphdr, struct udphdr udphdr, uint8_t *payload, int payloadlen) {
    char buf[IP_MAXPACKET];
    char *ptr;
    int chksumlen = 0;
    int i;

    ptr = &buf[0];  // ptr points to beginning of buffer buf

    // Copy source IP address into buf (128 bits)
    memcpy (ptr, &iphdr.ip6_src.s6_addr, sizeof (iphdr.ip6_src.s6_addr));
    ptr += sizeof (iphdr.ip6_src.s6_addr);
    chksumlen += sizeof (iphdr.ip6_src.s6_addr);

    // Copy destination IP address into buf (128 bits)
    memcpy (ptr, &iphdr.ip6_dst.s6_addr, sizeof (iphdr.ip6_dst.s6_addr));
    ptr += sizeof (iphdr.ip6_dst.s6_addr);
    chksumlen += sizeof (iphdr.ip6_dst.s6_addr);

    // Copy UDP length into buf (32 bits)
    memcpy (ptr, &udphdr.len, sizeof (udphdr.len));
    ptr += sizeof (udphdr.len);
    chksumlen += sizeof (udphdr.len);

    // Copy zero field to buf (24 bits)
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 3;

    // Copy next header field to buf (8 bits)
    memcpy (ptr, &iphdr.ip6_nxt, sizeof (iphdr.ip6_nxt));
    ptr += sizeof (iphdr.ip6_nxt);
    chksumlen += sizeof (iphdr.ip6_nxt);

    // Copy UDP source port to buf (16 bits)
    memcpy (ptr, &udphdr.source, sizeof (udphdr.source));
    ptr += sizeof (udphdr.source);
    chksumlen += sizeof (udphdr.source);

    // Copy UDP destination port to buf (16 bits)
    memcpy (ptr, &udphdr.dest, sizeof (udphdr.dest));
    ptr += sizeof (udphdr.dest);
    chksumlen += sizeof (udphdr.dest);

    // Copy UDP length again to buf (16 bits)
    memcpy (ptr, &udphdr.len, sizeof (udphdr.len));
    ptr += sizeof (udphdr.len);
    chksumlen += sizeof (udphdr.len);

    // Copy UDP checksum to buf (16 bits)
    // Zero, since we don't know it yet
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 2;

    // Copy payload to buf
    memcpy (ptr, payload, payloadlen * sizeof (uint8_t));
    ptr += payloadlen;
    chksumlen += payloadlen;

    // Pad to the next 16-bit boundary
    for (i=0; i<payloadlen%2; i++, ptr++) {
        *ptr = 0;
        ptr++;
        chksumlen++;
    }

    return checksum ((uint16_t *) buf, chksumlen);
}