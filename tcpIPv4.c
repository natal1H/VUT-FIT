#include "tcpIPv4.h"

int get_filter_expr_tcpIPv4(int port, char *filter_expr) {
    char port_str[5];
    sprintf(port_str, "%d", port);

    strcpy(filter_expr, "tcp src port ");
    strcat(filter_expr, port_str);
    strcat(filter_expr, " and tcp dst port 42");

    printf("Filter expression: %s\n", filter_expr);

}

int tcp_IPv4_port_scan(pcap_t *handle, int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip) {
    printf("TCP port scan (IPv4)\n");

    printf("Interface: %s\n", (interface == NULL) ? "null" : interface);
    printf("IP address: %s\n", (dest_address == NULL) ? "null" : dest_address);
    printf("IP address: %s\n", (source_address == NULL) ? "null" : source_address);

    // NECESSARY DECLARATIONS
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);	/* open raw socket */
    char datagram[4096];	/* this buffer will contain ip header, tcp header,
			   and payload. we'll point an ip header structure
			   at its beginning, and a tcp header structure after
			   that to write the header values into it */
    char *data, *pseudogram;
    //IP header
    struct iphdr *iph = (struct iphdr *) datagram;
    //TCP header
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in sin; // the sockaddr_in containing the dest. address is used in sendto() to determine the datagrams path
    //Data part
    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data , "Nothing suspicious here...");
    struct pseudo_header psh;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr (dest_address);

    memset (datagram, 0, 4096);	/* zero out the buffer */

    // IP header fill in
    fill_IP_header(iph, sin, data);

    //IP checksum
    iph->check = csum ((unsigned short *) datagram, iph->tot_len);

    //Now the TCP checksum
    fill_pseudo_header(&psh, sin, data);

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    pseudogram = (char *) malloc(psize);

    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));

    char *filter_expr = (char *) malloc(sizeof(char) * 38);
    struct bpf_program filter;

    printf("TCP ports:\n");
    for (int i = 0; i < num_ports; i++) {
        int dest_port = tcp_ports[i];
        printf("port %d\n", dest_port);

        sin.sin_port = htons (dest_port);

        // TCP header fill in
        fill_TCP_header(tcph, dest_port);

        memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));

        tcph->check = csum( (unsigned short*) pseudogram , psize);

        //IP_HDRINCL to tell the kernel that headers are included in the packet
        int one = 1;
        const int *val = &one;

        if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0) {
            perror("Error setting IP_HDRINCL");
            exit(0);
        }

        get_filter_expr_tcpIPv4(dest_port, filter_expr);
        if (pcap_compile(handle, &filter, filter_expr, 0, ip) == -1) {
            printf("Bad filter - %s\n", pcap_geterr(handle));
            return 2;
        }
        if (pcap_setfilter(handle, &filter) == -1) {
            printf("Error setting filter - %s\n", pcap_geterr(handle));
            return 2;
        }
        printf("filter set\n");

        struct pcap_pkthdr header; // Header that pcap gives us
        const u_char *packet; // Received raw data

        //Send the packet
        if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
            perror("sendto failed");
        }
        //Data send successfully
        else {
            printf ("Packet send to port %d \n" , dest_port);
        }

        // Grab packet
        packet = pcap_next(handle, &header);
        // print its lenght
        printf("Jacked a packet with length of %d\n", header.len);

        const struct sniff_ethernet *ethernet; /* The ethernet header */
        const struct sniff_ip *ip; /* The IP header */
        const struct sniff_tcp *tcp; /* The TCP header */
        const char *payload; /* Packet payload */

        u_int size_ip;
        u_int size_tcp;

        ethernet = (struct sniff_ethernet*)(packet);
        ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
        size_ip = IP_HL(ip)*4;
        if (size_ip < 20) {
            printf("   * Invalid IP header length: %u bytes\n", size_ip);
            return -1;
        }
        tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp)*4;
        if (size_tcp < 20) {
            printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
            return -1;
        }
        payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

        /* print source and destination IP addresses */
        printf("       From: %s\n", inet_ntoa(ip->ip_src));
        printf("         To: %s\n", inet_ntoa(ip->ip_dst));
        printf("      Flags: %x\n", tcp->th_flags);

        if (is_open_port(tcp->th_flags)) {
            printf("port %d: open\n", dest_port);
        }
        else if (is_closed_port(tcp->th_flags)) {
            printf("port %d: closed\n", dest_port);
        }
    }

    return 0;
}

bool is_open_port(u_char th_flags) {
    return ((th_flags & TH_SYN) && (th_flags & TH_ACK));
}

bool is_closed_port(u_char th_flags) {
    return ((th_flags & TH_RST) && (th_flags & TH_ACK));
}

void fill_IP_header(struct iphdr *iph, struct sockaddr_in sin, char *data) {
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
    iph->id = htonl (54321);	//Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;		//Set to 0 before calculating checksum
    iph->saddr = inet_addr ("127.0.0.1");	//Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;
}

void fill_TCP_header(struct tcphdr *tcph, int dest_port) {
    tcph->source = htons(42);
    tcph->dest = htons(dest_port);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;	//tcp header size
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840);	/* maximum allowed window size */
    tcph->check = 0;	//leave checksum 0 now, filled later by pseudo header
    tcph->urg_ptr = 0;
}

void fill_pseudo_header(struct pseudo_header *psh, struct sockaddr_in sin, char *data) {
    psh->source_address = inet_addr("127.0.0.1");
    psh->dest_address = sin.sin_addr.s_addr;
    psh->placeholder = 0;
    psh->protocol = IPPROTO_TCP;
    psh->tcp_length = htons(sizeof(struct tcphdr) + strlen(data) );
}

unsigned short csum(unsigned short *ptr,int nbytes) {
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