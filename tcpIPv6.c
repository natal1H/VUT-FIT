#include "tcpIPv6.h"

void get_filter_expr_tcpIPv6(int port, char *filter_expr) {
    char port_str[5];
    sprintf(port_str, "%d", port);

    strcpy(filter_expr, "tcp src port ");
    strcat(filter_expr, port_str);
    strcat(filter_expr, " and tcp dst port 42");
}

int tcp_IPv6_port_scan(int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *selected_interface, bpf_u_int32 ip) {

    int i, status, frame_length, sd, bytes, *tcp_flags;
    char *interface, *target, *src_ip, *dst_ip;
    struct ip6_hdr iphdr;
    struct tcphdr tcphdr;
    uint8_t *src_mac, *dst_mac, *ether_frame;
    struct addrinfo hints, *res;
    struct sockaddr_in6 *ipv6;
    struct sockaddr_ll device;
    struct ifreq ifr;
    void *tmp;

    // Allocate memory for various arrays.
    src_mac = allocate_ustrmem (6);
    dst_mac = allocate_ustrmem (6);
    ether_frame = allocate_ustrmem (IP_MAXPACKET);
    interface = allocate_strmem (40);
    target = allocate_strmem (INET6_ADDRSTRLEN);
    src_ip = allocate_strmem (INET6_ADDRSTRLEN);
    dst_ip = allocate_strmem (INET6_ADDRSTRLEN);
    tcp_flags = allocate_intmem (8);

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
    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_RAW;
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

    // IPv6 header

    // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    iphdr.ip6_flow = htonl ((6 << 28) | (0 << 20) | 0);

    // Payload length (16 bits): TCP header
    iphdr.ip6_plen = htons (TCP_HDRLEN);

    // Next header (8 bits): 6 for TCP
    iphdr.ip6_nxt = IPPROTO_TCP;

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
    // Ethernet frame length = ethernet header (MAC + MAC + ethernet type) + ethernet data (IP header + TCP header)
    frame_length = 6 + 6 + 2 + IP6_HDRLEN + TCP_HDRLEN;

    // Destination and Source MAC addresses
    memcpy (ether_frame, dst_mac, 6 * sizeof (uint8_t));
    memcpy (ether_frame + 6, src_mac, 6 * sizeof (uint8_t));

    // Next is ethernet type code (ETH_P_IPV6 for IPv6).
    // http://www.iana.org/assignments/ethernet-numbers
    ether_frame[12] = ETH_P_IPV6 / 256;
    ether_frame[13] = ETH_P_IPV6 % 256;

    // Next is ethernet frame data (IPv6 header + TCP header).
    // IPv6 header
    memcpy (ether_frame + ETH_HDRLEN, &iphdr, IP6_HDRLEN * sizeof (uint8_t));


    char *filter_expr = (char *) malloc(sizeof(char) * 38);
    struct bpf_program filter;

    // Submit request for a raw socket descriptor.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
        perror ("socket() failed ");
        exit (EXIT_FAILURE);
    }


    for (int curr_port = 0; curr_port < num_ports; curr_port++) {
        int dest_port = tcp_ports[curr_port];

        // TCP header
        // Source port number (16 bits)
        tcphdr.th_sport = htons (42);
        // Destination port number (16 bits)
        tcphdr.th_dport = htons (dest_port);
        // Sequence number (32 bits)
        tcphdr.th_seq = htonl (0);
        // Acknowledgement number (32 bits): 0 in first packet of SYN/ACK process
        tcphdr.th_ack = htonl (0);
        // Reserved (4 bits): should be 0
        tcphdr.th_x2 = 0;
        // Data offset (4 bits): size of TCP header in 32-bit words
        tcphdr.th_off = TCP_HDRLEN / 4;
        // Flags (8 bits)
        // FIN flag (1 bit)
        tcp_flags[0] = 0;
        // SYN flag (1 bit): set to 1
        tcp_flags[1] = 1;
        // RST flag (1 bit)
        tcp_flags[2] = 0;
        // PSH flag (1 bit)
        tcp_flags[3] = 0;
        // ACK flag (1 bit)
        tcp_flags[4] = 0;
        // URG flag (1 bit)
        tcp_flags[5] = 0;
        // ECE flag (1 bit)
        tcp_flags[6] = 0;
        // CWR flag (1 bit)
        tcp_flags[7] = 0;
        tcphdr.th_flags = 0;
        for (i=0; i<8; i++) {
            tcphdr.th_flags += (tcp_flags[i] << i);
        }
        // Window size (16 bits)
        tcphdr.th_win = htons (65535);
        // Urgent pointer (16 bits): 0 (only valid if URG flag is set)
        tcphdr.th_urp = htons (0);
        // TCP checksum (16 bits)
        tcphdr.th_sum = tcp6_checksum (iphdr, tcphdr);

        // TCP header
        memcpy (ether_frame + ETH_HDRLEN + IP6_HDRLEN, &tcphdr, TCP_HDRLEN * sizeof (uint8_t));


        get_filter_expr_tcpIPv6(dest_port, filter_expr);
        if (pcap_compile(handle, &filter, filter_expr, 0, ip) == -1) {
            fprintf(stderr, "Errror! Bad TCP filter - %s\n", pcap_geterr(handle));
            return 1;
        }
        if (pcap_setfilter(handle, &filter) == -1) {
            fprintf(stderr, "Error setting filter - %s\n", pcap_geterr(handle));
            return 1;
        }


        // Send ethernet frame to socket.
        if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
            perror ("sendto() failed");
            exit (EXIT_FAILURE);
        }

        // Grab packet
        int *port_ptr = &dest_port;
        alarm(PCAP_TIMEOUT);
        signal(SIGALRM, alarm_handler);

        int ret = pcap_loop(handle, 1, grab_packet_tcpIPv6, (u_char *) port_ptr);
        if (ret == -1) {
            fprintf(stderr, "Error! An error occurred in loop\n"); // No need to exit whole program
        }
        else if (ret == -2) {
            // pcap_breakloop was called - timeout
            // send another packet - if timeout again happens, it's a filtered port
            if (sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device)) <= 0) {
                if (sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device)) <= 0)
                    fprintf(stderr, "Error! Failed to send packet to port %d twice, this port will not be tested.\n", dest_port);
            }
            // Grab packet
            alarm(PCAP_TIMEOUT);
            signal(SIGALRM, alarm_handler);

            int second_ret = pcap_loop(handle, 1, grab_packet, (u_char *) port_ptr);
            if (second_ret == -1) {
                fprintf(stderr, "Error! An error occurred in loop\n"); // No need to exit whole program
            }
            else if (second_ret == -2) {
                // Filtered port
                printf("%d/tcp\t filtered\n", dest_port);
            }

        }

    }

    // Close socket descriptor.
    close (sd);

    // Free allocated memory.
    free (src_mac);
    free (dst_mac);
    free (ether_frame);
    free (interface);
    free (target);
    free (src_ip);
    free (dst_ip);
    free (tcp_flags);

    return (EXIT_SUCCESS);
}

void grab_packet_tcpIPv6(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet) {
    int *checked_port = (int *) args;
    //const struct ip6_hdr *ip; // The IP header
    const struct sniff_tcp *tcp; // The TCP header
    u_int size_ip = 40;
    //ip = (struct ip6_hdr *) (packet + SIZE_ETHERNET);
    //size_ip = IP_HL(ip) * 4;
    tcp = (struct sniff_tcp *) (packet + SIZE_ETHERNET + size_ip);

    if (is_open_port(tcp->th_flags)) {
        printf("%d/tcp\t open\n", *checked_port);
    } else if (is_closed_port(tcp->th_flags)) {
        printf("%d/tcp\t closed\n", *checked_port);
    }
}

// Computing the internet checksum (RFC 1071).
// Note that the internet checksum does not preclude collisions.
uint16_t
checksum (uint16_t *addr, int len)
{
    int count = len;
    register uint32_t sum = 0;
    uint16_t answer = 0;

    // Sum up 2-byte values until none or only one byte left.
    while (count > 1) {
        sum += *(addr++);
        count -= 2;
    }

    // Add left-over byte, if any.
    if (count > 0) {
        sum += *(uint8_t *) addr;
    }

    // Fold 32-bit sum into 16 bits; we lose information by doing this,
    // increasing the chances of a collision.
    // sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    // Checksum is one's compliment of sum.
    answer = ~sum;

    return (answer);
}

// Build IPv6 TCP pseudo-header and call checksum function (Section 8.1 of RFC 2460).
uint16_t
tcp6_checksum (struct ip6_hdr iphdr, struct tcphdr tcphdr)
{
    uint32_t lvalue;
    char buf[IP_MAXPACKET], cvalue;
    char *ptr;
    int chksumlen = 0;

    ptr = &buf[0];  // ptr points to beginning of buffer buf

    // Copy source IP address into buf (128 bits)
    memcpy (ptr, &iphdr.ip6_src, sizeof (iphdr.ip6_src));
    ptr += sizeof (iphdr.ip6_src);
    chksumlen += sizeof (iphdr.ip6_src);

    // Copy destination IP address into buf (128 bits)
    memcpy (ptr, &iphdr.ip6_dst, sizeof (iphdr.ip6_dst));
    ptr += sizeof (iphdr.ip6_dst);
    chksumlen += sizeof (iphdr.ip6_dst);

    // Copy TCP length to buf (32 bits)
    lvalue = htonl (sizeof (tcphdr));
    memcpy (ptr, &lvalue, sizeof (lvalue));
    ptr += sizeof (lvalue);
    chksumlen += sizeof (lvalue);

    // Copy zero field to buf (24 bits)
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 3;

    // Copy next header field to buf (8 bits)
    memcpy (ptr, &iphdr.ip6_nxt, sizeof (iphdr.ip6_nxt));
    ptr += sizeof (iphdr.ip6_nxt);
    chksumlen += sizeof (iphdr.ip6_nxt);

    // Copy TCP source port to buf (16 bits)
    memcpy (ptr, &tcphdr.th_sport, sizeof (tcphdr.th_sport));
    ptr += sizeof (tcphdr.th_sport);
    chksumlen += sizeof (tcphdr.th_sport);

    // Copy TCP destination port to buf (16 bits)
    memcpy (ptr, &tcphdr.th_dport, sizeof (tcphdr.th_dport));
    ptr += sizeof (tcphdr.th_dport);
    chksumlen += sizeof (tcphdr.th_dport);

    // Copy sequence number to buf (32 bits)
    memcpy (ptr, &tcphdr.th_seq, sizeof (tcphdr.th_seq));
    ptr += sizeof (tcphdr.th_seq);
    chksumlen += sizeof (tcphdr.th_seq);

    // Copy acknowledgement number to buf (32 bits)
    memcpy (ptr, &tcphdr.th_ack, sizeof (tcphdr.th_ack));
    ptr += sizeof (tcphdr.th_ack);
    chksumlen += sizeof (tcphdr.th_ack);

    // Copy data offset to buf (4 bits) and
    // copy reserved bits to buf (4 bits)
    cvalue = (tcphdr.th_off << 4) + tcphdr.th_x2;
    memcpy (ptr, &cvalue, sizeof (cvalue));
    ptr += sizeof (cvalue);
    chksumlen += sizeof (cvalue);

    // Copy TCP flags to buf (8 bits)
    memcpy (ptr, &tcphdr.th_flags, sizeof (tcphdr.th_flags));
    ptr += sizeof (tcphdr.th_flags);
    chksumlen += sizeof (tcphdr.th_flags);

    // Copy TCP window size to buf (16 bits)
    memcpy (ptr, &tcphdr.th_win, sizeof (tcphdr.th_win));
    ptr += sizeof (tcphdr.th_win);
    chksumlen += sizeof (tcphdr.th_win);

    // Copy TCP checksum to buf (16 bits)
    // Zero, since we don't know it yet
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 2;

    // Copy urgent pointer to buf (16 bits)
    memcpy (ptr, &tcphdr.th_urp, sizeof (tcphdr.th_urp));
    ptr += sizeof (tcphdr.th_urp);
    chksumlen += sizeof (tcphdr.th_urp);

    return checksum ((uint16_t *) buf, chksumlen);
}

// Allocate memory for an array of chars.
char *
allocate_strmem (int len)
{
    void *tmp;

    if (len <= 0) {
        fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_strmem().\n", len);
        exit (EXIT_FAILURE);
    }

    tmp = (char *) malloc (len * sizeof (char));
    if (tmp != NULL) {
        memset (tmp, 0, len * sizeof (char));
        return (tmp);
    } else {
        fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_strmem().\n");
        exit (EXIT_FAILURE);
    }
}

// Allocate memory for an array of unsigned chars.
uint8_t *
allocate_ustrmem (int len)
{
    void *tmp;

    if (len <= 0) {
        fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_ustrmem().\n", len);
        exit (EXIT_FAILURE);
    }

    tmp = (uint8_t *) malloc (len * sizeof (uint8_t));
    if (tmp != NULL) {
        memset (tmp, 0, len * sizeof (uint8_t));
        return (tmp);
    } else {
        fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_ustrmem().\n");
        exit (EXIT_FAILURE);
    }
}

// Allocate memory for an array of ints.
int *
allocate_intmem (int len)
{
    void *tmp;

    if (len <= 0) {
        fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_intmem().\n", len);
        exit (EXIT_FAILURE);
    }

    tmp = (int *) malloc (len * sizeof (int));
    if (tmp != NULL) {
        memset (tmp, 0, len * sizeof (int));
        return (tmp);
    } else {
        fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_intmem().\n");
        exit (EXIT_FAILURE);
    }
}