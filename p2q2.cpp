#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

using namespace std;
int i;


#define PCAP_FILE "output.pcap"

void packet_handler(const struct pcap_pkthdr *header, const u_char *packet) {
    struct iphdr *ip_header = (struct iphdr *)(packet + 14);
    struct in_addr src_ip, dst_ip;
    src_ip.s_addr = ip_header->saddr;
    dst_ip.s_addr = ip_header->daddr;

    u_int8_t protocol = ip_header->protocol;

    if (protocol == IPPROTO_TCP) {
        struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + ip_header->ihl * 4);

        unsigned short checksum = ntohs(tcp_header->check);
        unsigned short urgent_pointer = ntohs(tcp_header->urg_ptr);
        uint32_t sequence_number = ntohl(tcp_header->seq);

        if ((checksum & 0xFF00) == 0xb500 && urgent_pointer == 0 && (sequence_number%10000) == 6183) {
            cout << "Protocol: TCP"<<endl;
            cout << "Source IP: " << inet_ntoa(src_ip) << ", Destination IP: " << inet_ntoa(dst_ip) << ", Checksum: 0x" << hex << checksum << dec << ", Sequence number: " <<sequence_number << endl;
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
    i=0;
    
    while (pcap_next_ex(handle, &header, &packet) == 1) {
        i++;
        packet_handler(header, packet);
    }

    pcap_close(handle);
    return 0;
}
