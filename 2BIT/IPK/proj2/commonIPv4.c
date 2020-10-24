#include "commonIPv4.h"

/*
 * Checksum function
 *
 * Taken from: https://github.com/seifzadeh/c-network-programming-best-snipts/blob/master/Programming%20raw%20udp%20sockets%20in%20C%20on%20Linux
 */
unsigned short csum(unsigned short *ptr,int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while (nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if (nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

/*
 * Fill in pseudo header
 */
void fill_pseudo_header(struct pseudo_header *psh, struct sockaddr_in sin, char *data, char *source_ip,  uint8_t protocol, size_t size_tcp_udp) {
    psh->source_address = inet_addr(source_ip);
    psh->dest_address = sin.sin_addr.s_addr;
    psh->placeholder = 0;
    psh->protocol = protocol;
    psh->tcp_length = htons(size_tcp_udp + strlen(data) );
}

/*
 * Fill in IP header
 */
void fill_IP_header(struct iphdr *iph, struct sockaddr_in sin, char *data, char *source_ip, u_int8_t protocol, size_t size_tcp_udp) {
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + size_tcp_udp + strlen(data);
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = protocol;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr (source_ip);    //Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;
}

/*
 * Alarm program when time set for pcap_loop timeout passed
 */
void alarm_handler(int sig) {
    pcap_breakloop(handle);
}