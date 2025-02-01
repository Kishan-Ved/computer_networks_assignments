#include <pcap.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <map>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <cstring>

using namespace std;

struct Flow {
    string src_ip;
    uint16_t src_port;
    string dst_ip;
    uint16_t dst_port;
    uint32_t data_transferred;

    bool operator<(const Flow& other) const {
        return (src_ip < other.src_ip) ||
               (src_ip == other.src_ip && src_port < other.src_port) ||
               (src_ip == other.src_ip && src_port == other.src_port && dst_ip < other.dst_ip) ||
               (src_ip == other.src_ip && src_port == other.src_port && dst_ip == other.dst_ip && dst_port < other.dst_port);
    }
};

void packet_handler(const struct pcap_pkthdr *header, const u_char *packet,
                    map<string, int>& src_ip_flows, map<string, int>& dst_ip_flows,
                    map<Flow, uint32_t>& flow_data) {

    struct ip *ip_hdr = (struct ip *)(packet + 14);

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_hdr->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_hdr->ip_dst), dst_ip, INET_ADDRSTRLEN);

    if (strcmp(src_ip, "127.0.0.1") == 0 || strcmp(dst_ip, "127.0.0.1") == 0) {
        return;
    }

    src_ip_flows[src_ip]++;
    dst_ip_flows[dst_ip]++;

    if (ip_hdr->ip_p == IPPROTO_TCP) {
        struct tcphdr *tcp_hdr = (struct tcphdr *)(packet + 14 + (ip_hdr->ip_hl * 4));
        uint16_t src_port = ntohs(tcp_hdr->th_sport);
        uint16_t dst_port = ntohs(tcp_hdr->th_dport);

        Flow flow = {src_ip, src_port, dst_ip, dst_port, header->len};
        flow_data[flow] += header->len;
    }
    else if (ip_hdr->ip_p == IPPROTO_UDP) {
        struct udphdr *udp_hdr = (struct udphdr *)(packet + 14 + (ip_hdr->ip_hl * 4));
        uint16_t src_port = ntohs(udp_hdr->uh_sport);
        uint16_t dst_port = ntohs(udp_hdr->uh_dport);

        Flow flow = {src_ip, src_port, dst_ip, dst_port, header->len};
        flow_data[flow] += header->len;
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

    map<string, int> src_ip_flows;
    map<string, int> dst_ip_flows;
    map<Flow, uint32_t> flow_data;

    struct pcap_pkthdr header;
    const u_char *packet;
    while ((packet = pcap_next(handle, &header)) != nullptr) {
        packet_handler(&header, packet, src_ip_flows, dst_ip_flows, flow_data);
    }

    pcap_close(handle);

    cout << "Source IP Flows:" << endl;
    for (const auto& entry : src_ip_flows) {
        cout << entry.first << ": " << entry.second << " flows" << endl;
    }

    cout << "\nDestination IP Flows:" << endl;
    for (const auto& entry : dst_ip_flows) {
        cout << entry.first << ": " << entry.second << " flows" << endl;
    }

    Flow max_flow;
    uint32_t max_data = 0;

    for (const auto& entry : flow_data) {
        if (entry.second > max_data) {
            max_flow = entry.first;
            max_data = entry.second;
        }
    }

    cout << "\nFlow with the most data transferred:" << endl;
    cout << "Source IP: " << max_flow.src_ip << ":" << max_flow.src_port
         << " -> Destination IP: " << max_flow.dst_ip << ":" << max_flow.dst_port
         << " transferred " << max_data << " bytes" << endl;

    return 0;
}
