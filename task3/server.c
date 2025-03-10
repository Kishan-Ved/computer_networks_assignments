#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <nagle> <quickack>\n", argv[0]);
        exit(1);
    }
    int nagle = atoi(argv[1]);  // 0: enable Nagle, 1: disable
    int quickack = atoi(argv[2]);  // 0: enable Delayed-ACK, 1: disable

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    int total_received = 0;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow port reuse
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    // Configure Nagle's Algorithm
    int flag_nagle = !nagle;  // 0: enable Nagle, 1: disable
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &flag_nagle, sizeof(int))) {
        perror("setsockopt TCP_NODELAY");
        exit(EXIT_FAILURE);
    }

    // Configure Delayed-ACK
    int flag_quickack = quickack;  // 0: enable Delayed-ACK, 1: disable
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_QUICKACK, &flag_quickack, sizeof(int))) {
        perror("setsockopt TCP_QUICKACK");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind and listen
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Apply settings to accepted socket
    if (setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, &flag_nagle, sizeof(int))) {
        perror("setsockopt TCP_NODELAY on accepted socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(new_socket, IPPROTO_TCP, TCP_QUICKACK, &flag_quickack, sizeof(int))) {
        perror("setsockopt TCP_QUICKACK on accepted socket");
        exit(EXIT_FAILURE);
    }

    // Receive 4 KB
    while (total_received < 4096) {
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        total_received += valread;
    }

    printf("Received %d bytes\n", total_received);
    close(new_socket);
    close(server_fd);
    return 0;
}