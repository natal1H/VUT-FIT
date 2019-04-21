#include "tcpIPv4.h"

/***************************************************************************************
 * Perform TCP port scan on selected ports (version with IPv4 address)
 *
 *    Disclaimer: Sending TCP packets was done according to this tutorial:
 *
 *    Title: Code raw sockets in C on Linux
 *    Author: Silver Moon https://www.binarytides.com/author/admin/
 *    Availability: https://www.binarytides.com/raw-sockets-c-code-linux/
 ***************************************************************************************/
int tcp_IPv4_port_scan(int *tcp_ports, int num_ports, char *dest_address, char *source_address, char *interface, bpf_u_int32 ip) {
    // NECESSARY DECLARATIONS
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP); // open raw socket
    char datagram[4096];
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
    sin.sin_addr.s_addr = inet_addr(dest_address);

    memset (datagram, 0, 4096);	/* zero out the buffer */

    // IP header fill in
    fill_IP_header(iph, sin, data, source_address, IPPROTO_TCP, sizeof(struct tcphdr));

    // IP checksum
    iph->check = csum((unsigned short *) datagram, iph->tot_len);

    // Fill in pseudo header
    fill_pseudo_header(&psh, sin, data, source_address, IPPROTO_TCP, sizeof(struct tcphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    pseudogram = (char *) malloc(psize);

    memcpy(pseudogram, (char*) &psh , sizeof (struct pseudo_header));

    char *filter_expr = (char *) malloc(sizeof(char) * 38);
    struct bpf_program filter;

    for (int i = 0; i < num_ports; i++) {
        int dest_port = tcp_ports[i];

        sin.sin_port = htons (dest_port);

        // TCP header fill in
        fill_TCP_header(tcph, dest_port);

        memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));

        tcph->check = csum( (unsigned short*) pseudogram , psize);

        int one = 1;
        const int *val = &one;
        //IP_HDRINCL to tell the kernel that headers are included in the packet
        if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0) {
            fprintf(stderr, "Error! Could not set up socket.\n");
            return 1;
        }

        // Prepare packet filter
        get_filter_expr_tcpIPv4(dest_port, filter_expr);
        if (pcap_compile(handle, &filter, filter_expr, 0, ip) == -1) {
            fprintf(stderr, "Errror! Bad TCP filter - %s\n", pcap_geterr(handle));
            return 1;
        }
        if (pcap_setfilter(handle, &filter) == -1) {
            fprintf(stderr, "Error setting filter - %s\n", pcap_geterr(handle));
            return 1;
        }

        //Send the packet
        if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
            if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
                fprintf(stderr, "Error! Failed to send packet to port %d twice, this port will not be tested.\n", dest_port);
        }
        //Data send successfully

        // Grab packet
        int *port_ptr = &dest_port;
        alarm(PCAP_TIMEOUT);
        signal(SIGALRM, alarm_handler);

        int ret = pcap_loop(handle, 1, grab_tcp_packet, (u_char *) port_ptr);
        if (ret == -1) {
            fprintf(stderr, "Error! An error occurred in loop\n"); // No need to exit whole program
        }
        else if (ret == -2) {
            // pcap_breakloop was called - timeout
            // send another packet - if timeout again happens, it's a filtered port
            if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
                if (sendto (s, datagram, iph->tot_len ,	0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
                    fprintf(stderr, "Error! Failed to send packet to port %d twice, this port will not be tested.\n", dest_port);
            }
            // Grab packet
            alarm(PCAP_TIMEOUT);
            signal(SIGALRM, alarm_handler);

            int second_ret = pcap_loop(handle, 1, grab_tcp_packet, (u_char *) port_ptr);
            if (second_ret == -1) {
                fprintf(stderr, "Error! An error occurred in loop\n"); // No need to exit whole program
            }
            else if (second_ret == -2) {
                // Filtered port
                printf("%d/tcp\t filtered\n", dest_port);
            }

        }

    }

    close(s);

    return 0;
}

/*
 * Get filter expression
 */
void get_filter_expr_tcpIPv4(int port, char *filter_expr) {
    char port_str[5];
    sprintf(port_str, "%d", port);

    strcpy(filter_expr, "tcp src port ");
    strcat(filter_expr, port_str);
    strcat(filter_expr, " and tcp dst port 42");
}

/***************************************************************************************
*    Disclaimer: Typecasting received packet (along with used structures) was done according to this tutorial:
*
*    Title: Programming with pcap
*    Author: Tim Carstens
*    Availability: https://www.tcpdump.org/pcap.html
***************************************************************************************/
void grab_tcp_packet(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char *packet) {
    int *checked_port = (int *) args;
    const struct sniff_ip *ip; // The IP header
    const struct sniff_tcp *tcp; // The TCP header
    u_int size_ip;
    ip = (struct sniff_ip *) (packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip) * 4;
    tcp = (struct sniff_tcp *) (packet + SIZE_ETHERNET + size_ip);

    if (is_open_port(tcp->th_flags)) {
        printf("%d/tcp\t open\n", *checked_port);
    } else if (is_closed_port(tcp->th_flags)) {
        printf("%d/tcp\t closed\n", *checked_port);
    }
}

/*
 * Function to analyze TCP flags and decide if port is open
 */
bool is_open_port(u_char th_flags) {
    return ((th_flags & TH_SYN) && (th_flags & TH_ACK));
}

/*
 * Function to analyze TCP flags and decide if port is closed
 */
bool is_closed_port(u_char th_flags) {
    return ((th_flags & TH_RST) && (th_flags & TH_ACK));
}

/*
 * Fill out TCP header
 */
void fill_TCP_header(struct tcphdr *tcph, int dest_port) {
    tcph->source = htons(SRC_PORT);
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
    tcph->window = htons (5840);
    tcph->check = 0;
    tcph->urg_ptr = 0;
}
