#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip6.h>
#include <sys/socket.h>
#include <pcap.h>
#include <signal.h>

#define BUFFER_SIZE 65536

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    stop = 1;
}

void process_packet(unsigned char *buffer, int packet_size, pcap_dumper_t *pcap_dumper) {
    struct pcap_pkthdr header;
    header.ts.tv_sec = time(NULL);
    header.ts.tv_usec = 0;
    header.caplen = packet_size;
    header.len = packet_size;

    pcap_dump((u_char*)pcap_dumper, &header, buffer);
}

int main() {
    int raw_socket;
    unsigned char buffer[BUFFER_SIZE];

    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    const char *interface = "lo";
    if (setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface)) < 0) {
        perror("Failed to bind socket to interface");
        return 1;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap_handle = pcap_open_dead(DLT_EN10MB, BUFFER_SIZE);
    if (!pcap_handle) {
        std::cerr << "Failed to initialize pcap handle" << std::endl;
        return 1;
    }

    pcap_dumper_t *pcap_dumper = pcap_dump_open(pcap_handle, "captured_packets.pcap");
    if (!pcap_dumper) {
        std::cerr << "Failed to open pcap file for writing" << std::endl;
        return 1;
    }

    std::cout << "Listening for packets on 'lo' interface..." << std::endl;

    signal(SIGINT, handle_sigint);

    while (!stop) {
        socklen_t addr_len;
        struct sockaddr saddr;
        int packet_size = recvfrom(raw_socket, buffer, BUFFER_SIZE, 0, &saddr, &addr_len);
        if (packet_size < 0) {
            perror("Packet reception failed");
            break;
        }
        process_packet(buffer, packet_size, pcap_dumper);
    }

    pcap_dump_close(pcap_dumper);
    pcap_close(pcap_handle);
    close(raw_socket);
    return 0;
}
