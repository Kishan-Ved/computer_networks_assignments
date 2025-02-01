#include <pcap.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <set>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

struct Flow {
    string src_ip;
    uint16_t src_port;
    string dst_ip;
    uint16_t dst_port;

    bool operator<(const Flow& other) const {
        return (src_ip < other.src_ip) || 
               (src_ip == other.src_ip && src_port < other.src_port) ||
               (src_ip == other.src_ip && src_port == other.src_port && dst_ip < other.dst_ip) ||
               (src_ip == other.src_ip && src_port == other.src_port && dst_ip == other.dst_ip && dst_port < other.dst_port);
    }
};

void packet_handler(const struct pcap_pkthdr *header, const u_char *packet, set<Flow>& unique_flows) {
    struct ip *ip_hdr = (struct ip *)(packet + 14);

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_hdr->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_hdr->ip_dst), dst_ip, INET_ADDRSTRLEN);

    if (strcmp(src_ip, "127.0.0.1") == 0 || strcmp(dst_ip, "127.0.0.1") == 0) {
        return;
    }

    if (ip_hdr->ip_p == IPPROTO_TCP) {
        struct tcphdr *tcp_hdr = (struct tcphdr *)(packet + 14 + (ip_hdr->ip_hl * 4));
        uint16_t src_port = ntohs(tcp_hdr->th_sport);
        uint16_t dst_port = ntohs(tcp_hdr->th_dport);

        Flow flow = {src_ip, src_port, dst_ip, dst_port};
        unique_flows.insert(flow);
    }
    else if (ip_hdr->ip_p == IPPROTO_UDP) {
        struct udphdr *udp_hdr = (struct udphdr *)(packet + 14 + (ip_hdr->ip_hl * 4));
        uint16_t src_port = ntohs(udp_hdr->uh_sport);
        uint16_t dst_port = ntohs(udp_hdr->uh_dport);

        Flow flow = {src_ip, src_port, dst_ip, dst_port};
        unique_flows.insert(flow);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <pcap_file>" << endl;
        return 1;
    }

    pcap_t *handle = pcap_open_offline(argv[1], nullptr);
    if (handle == nullptr) {
        cerr << "Error opening pcap file: " << pcap_geterr(handle) << endl;
        return 1;
    }

    set<Flow> unique_flows;

    struct pcap_pkthdr header;
    const u_char *packet;
    while ((packet = pcap_next(handle, &header)) != nullptr) {
        packet_handler(&header, packet, unique_flows);
    }

    pcap_close(handle);

    cout << "Number of unique source-destination pairs: " << unique_flows.size() << endl;

    return 0;
}
