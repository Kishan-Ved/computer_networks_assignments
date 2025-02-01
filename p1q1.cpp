#include <pcap.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <netinet/ip.h>
#include <netinet/in.h>

using namespace std;

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    vector<int>* packet_sizes = (vector<int>*) user_data;

    struct ip* ip_header = (struct ip*)(packet + 14);

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

    if (strcmp(src_ip, "127.0.0.1") == 0 || strcmp(dst_ip, "127.0.0.1") == 0) {
        return;
    }

    int packet_size = pkthdr->len;

    packet_sizes->push_back(packet_size);
}

void analyze_pcap(const char* filename) {
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];

    handle = pcap_open_offline(filename, errbuf);
    if (handle == nullptr) {
        cerr << "Error opening pcap file: " << errbuf << endl;
        return;
    }

    vector<int> packet_sizes;

    if (pcap_loop(handle, 0, packet_handler, (u_char*)&packet_sizes) < 0) {
        cerr << "Error reading pcap file: " << pcap_geterr(handle) << endl;
        pcap_close(handle);
        return;
    }

    pcap_close(handle);

    int total_data = accumulate(packet_sizes.begin(), packet_sizes.end(), 0);
    int total_packets = packet_sizes.size();

    int min_size = *min_element(packet_sizes.begin(), packet_sizes.end());
    int max_size = *max_element(packet_sizes.begin(), packet_sizes.end());
    double avg_size = total_data / static_cast<double>(total_packets);

    cout << "Total Data Transferred: " << total_data << " bytes" << endl;
    cout << "Total Packets: " << total_packets << endl;
    cout << "Minimum Packet Size: " << min_size << " bytes" << endl;
    cout << "Maximum Packet Size: " << max_size << " bytes" << endl;
    cout << "Average Packet Size: " << avg_size << " bytes" << endl;

    const int bin_size = 100;
    const int num_bins = (max_size / bin_size) + 1;

    vector<int> histogram(num_bins, 0);

    for (int size : packet_sizes) {
        int bin_index = size / bin_size;
        if (bin_index >= num_bins) bin_index = num_bins - 1;
        histogram[bin_index]++;
    }

    cout << "\nPacket Size Distribution:" << endl;
    for (int i = 0; i < num_bins; i++) {
        int bin_start = i * bin_size;
        int bin_end = bin_start + bin_size - 1;
        cout << "[" << bin_start << " - " << bin_end << "] bytes: " << histogram[i] << " packets" << endl;
    }
}

int main() {
    const char* filename = "output.pcap";
    analyze_pcap(filename);
    return 0;
}
