#include "udpIPv4.h"

unsigned short csum_udpIPv4(unsigned short *ptr,int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

int udp_IPv4_port_scan(int *udp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip) {
    //Create a raw socket of type IPPROTO
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s == -1) {
        //socket creation failed, may be because of non-root privileges
        fprintf(stderr, "Error! Failed to create raw UDP socket.\n");
        exit(ERR_UDP_SOCKET);
    }
    char datagram[4096], *data , *pseudogram; //Datagram to represent the packet
    memset (datagram, 0, 4096); //zero out the packet buffer
    struct iphdr *iph = (struct iphdr *) datagram; //IP header
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip)); //UDP header
    struct sockaddr_in sin; // the sockaddr_in containing the dest. address is used in sendto() to determine the datagrams path
    struct pseudo_header_udpIPv4 psh;

    data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr); //Data part
    strcpy(data , "Nothing suspicious here...");

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(dest_address);

    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr (source_address);    //Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;

    //Ip checksum
    iph->check = csum_udpIPv4((unsigned short *) datagram, iph->tot_len);

    //Now the UDP checksum using the pseudo header
    psh.source_address = inet_addr(source_address);
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(data) );

    char filter_expr[] = "icmp[0] = 3";
    struct bpf_program filter;
    if (pcap_compile(handle, &filter, filter_expr, 0, ip) == -1) {
        printf("Bad filter - %s\n", pcap_geterr(handle));
        return ERR_TCP_LIBPCAP;
    }
    if (pcap_setfilter(handle, &filter) == -1) {
        printf("Error setting filter - %s\n", pcap_geterr(handle));
        return ERR_TCP_LIBPCAP;
    }

    for (int i = 0; i < num_ports; i++) {
        int dest_port = udp_ports[i];

        sin.sin_port = htons(dest_port);


        //UDP header
        udph->source = htons (42);
        udph->dest = htons (dest_port);
        udph->len = htons(8 + strlen(data)); // header size
        udph->check = 0; //leave checksum 0 now, filled later by pseudo header

        int psize = sizeof(struct pseudo_header_udpIPv4) + sizeof(struct udphdr) + strlen(data);
        pseudogram = malloc(psize);

        memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header_udpIPv4));
        memcpy(pseudogram + sizeof(struct pseudo_header_udpIPv4) , udph , sizeof(struct udphdr) + strlen(data));

        udph->check = csum_udpIPv4( (unsigned short*) pseudogram , psize);

        //Send the packet
        if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
            fprintf(stderr, "Error! Failed to send UDP packet.\n");
        }
        // Date send successfully

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