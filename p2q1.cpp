#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

using namespace std;

int ans=0;

#define PCAP_FILE "output.pcap"

void packet_handler(const struct pcap_pkthdr *header, const u_char *packet) {
    struct iphdr *ip_header = (struct iphdr *)(packet + 14);
    struct in_addr src_ip, dst_ip;
    src_ip.s_addr = ip_header->saddr;
    dst_ip.s_addr = ip_header->daddr;

    u_int8_t protocol = ip_header->protocol;

    if (protocol == IPPROTO_TCP) {
        int src_port, dst_port;
        struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + ip_header->ihl * 4);
        src_port = ntohs(tcp_header->source);
        dst_port = ntohs(tcp_header->dest);
        uint32_t acknum = ntohl(tcp_header->ack_seq);
        int absdiff = abs(src_port - dst_port);

        if (absdiff == 54286 && acknum%10000 == 1203) {
            ans++;
            cout << "Protocol: TCP\n";
            cout << "Source IP: " << inet_ntoa(src_ip) << ", Destination IP: " << inet_ntoa(dst_ip)<< endl;
            cout << "Source Port: " << src_port << ", Destination Port: " << dst_port<< endl;
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

    cout<<"Total number of required packets: "<<ans<<endl;

    pcap_close(handle);
    return 0;
}
