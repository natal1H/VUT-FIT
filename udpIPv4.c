#include "udpIPv4.h"

int udp_IPv4_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip) {
    //Create a raw socket of type IPPROTO
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s == -1) {
        //socket creation failed, may be because of non-root privileges
        fprintf(stderr, "Error! Failed to create raw UDP socket.\n");
        exit(1);
    }
    char datagram[4096], *data , *pseudogram; //Datagram to represent the packet
    memset (datagram, 0, 4096); //zero out the packet buffer
    struct iphdr *iph = (struct iphdr *) datagram; //IP header
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip)); //UDP header
    struct sockaddr_in sin; // the sockaddr_in containing the dest. address is used in sendto() to determine the datagrams path
    struct pseudo_header psh;

    data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr); //Data part
    strcpy(data , "Nothing suspicious here...");

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(dest_address);

    //Fill in the IP Header
    fill_IP_header(iph, sin, data, source_address, IPPROTO_UDP, sizeof(struct udphdr));

    //Ip checksum
    iph->check = csum((unsigned short *) datagram, iph->tot_len);

    //Now the UDP checksum using the pseudo header
    fill_pseudo_header(&psh, sin, data, source_address, IPPROTO_UDP, sizeof(struct udphdr));

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

    for (int i = 0; i < num_ports; i++) {
        int dest_port = udp_ports[i];

        sin.sin_port = htons(dest_port);

        //UDP header
        fill_UDP_header(udph, dest_port, data);

        int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
        pseudogram = malloc(psize);

        memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
        memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(data));

        udph->check = csum((unsigned short*) pseudogram , psize);

        //Send the packet
        if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
            fprintf(stderr, "Error! Failed to send UDP packet.\n");
        }
        // Data send successfully

        // Grab packet
        int *port_ptr = &dest_port;
        alarm(PCAP_TIMEOUT);
        signal(SIGALRM, alarm_handler);

        int ret = pcap_loop(handle, 1, grab_udp_packet, (u_char *) port_ptr);
        if (ret == -1) {
            fprintf(stderr, "Error! An error occurred in loop\n"); // No need to exit whole program
        }
        else if (ret == -2) { // Breakloop
            // Port open or filtered
            printf("%d/udp\t open\n", dest_port);
        }
    }

    return 0;
}

void grab_udp_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet) {
    int *checked_port = (int *) args;
    printf("%d/udp\t closed\n", *checked_port);
}

void fill_UDP_header(struct udphdr *udph, int dest_port, char *data) {
    udph->source = htons(SRC_PORT);
    udph->dest = htons (dest_port);
    udph->len = htons(8 + strlen(data)); // header size
    udph->check = 0; //leave checksum 0 now, filled later by pseudo header
}