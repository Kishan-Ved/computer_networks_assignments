#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <cstring>

using namespace std;
int ans=0;

#define PCAP_FILE "8.pcap"

void packet_handler(const struct pcap_pkthdr *header, const u_char *packet) {
    struct iphdr *ip_header = (struct iphdr *)(packet + 14);
    struct in_addr src_ip, dst_ip;
    src_ip.s_addr = ip_header->saddr;
    dst_ip.s_addr = ip_header->daddr;

    char src_ip_str[INET_ADDRSTRLEN];
    char dst_ip_str[INET_ADDRSTRLEN];

    strcpy(src_ip_str, inet_ntoa(src_ip));
    strcpy(dst_ip_str, inet_ntoa(dst_ip));

    if (strcmp(src_ip_str, "127.0.0.1") == 0 || strcmp(dst_ip_str, "127.0.0.1") == 0) {
        return;
    }

    u_int8_t protocol = ip_header->protocol;

    if (protocol == IPPROTO_TCP) {
        struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + ip_header->ihl * 4);

        unsigned short checksum = ntohs(tcp_header->check);
        unsigned short urgent_pointer = ntohs(tcp_header->urg_ptr);
        uint32_t acknum = ntohl(tcp_header->ack_seq);
        if(1678999000 <= acknum && acknum <= 1700000000){
            ans++;
        }
    }
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_offline(PCAP_FILE, errbuf);
    if (!handle) {
        cerr << "Error opening file: " << errbuf << endl;
        return 1;
    }

    struct pcap_pkthdr *header;
    const u_char *packet;
    
    while (pcap_next_ex(handle, &header, &packet) == 1) {
        packet_handler(header, packet);
    }

    cout<<ans<<endl;

    pcap_close(handle);
    return 0;
}
