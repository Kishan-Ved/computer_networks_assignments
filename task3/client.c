#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "10.0.0.2"
#define FILE_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <nagle> <quickack>\n", argv[0]);
        exit(1);
    }
    int nagle = atoi(argv[1]);  // 0: enable Nagle, 1: disable
    int quickack = atoi(argv[2]);  // 0: enable Delayed-ACK, 1: disable

    int sock;
    struct sockaddr_in serv_addr;
    char buffer[FILE_SIZE] = {0};  // Dummy 4 KB data

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configure Nagle's Algorithm
    int flag_nagle = !nagle;  // 0: enable Nagle, 1: disable
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag_nagle, sizeof(int))) {
        perror("setsockopt TCP_NODELAY");
        exit(EXIT_FAILURE);
    }

    // Configure Delayed-ACK
    int flag_quickack = quickack;  // 0: enable Delayed-ACK, 1: disable
    if (setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, &flag_quickack, sizeof(int))) {
        perror("setsockopt TCP_QUICKACK");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // Send 4 KB
    int total_sent = 0;
    while (total_sent < FILE_SIZE) {
        int bytes_to_send = (FILE_SIZE - total_sent > 40) ? 40 : (FILE_SIZE - total_sent);
        int sent = send(sock, buffer + total_sent, bytes_to_send, 0);
        if (sent < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }
        total_sent += sent;
        usleep(500);
    }

    printf("Sent %d bytes\n", total_sent);
    close(sock);
    return 0;
}