#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define TFTP_PORT 69
#define BUFFER_SIZE 516  // TFTP packets are max 516 bytes

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *server = argv[1];
    const char *filename = argv[2];

    printf("Server: %s, File: %s\n", server, filename);

    // Resolve the server address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  // Support both IPv4 and IPv6
    hints.ai_socktype = SOCK_DGRAM;  // UDP socket

    char port[6];
    snprintf(port, sizeof(port), "%d", TFTP_PORT);  // Convert port to string

    if (getaddrinfo(server, port, &hints, &res) != 0) {
        perror("Failed to resolve server address");
        return EXIT_FAILURE;
    }

    // Create a UDP socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("Socket creation failed");
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    // Connect the socket to the server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Failed to connect socket");
        freeaddrinfo(res);
        close(sockfd);
        return EXIT_FAILURE;
    }

    freeaddrinfo(res);
    printf("Socket connected to the server.\n");

    // Construct a TFTP Write Request (WRQ) packet
    char buffer[BUFFER_SIZE];
    int wrq_len = 0;

    buffer[wrq_len++] = 0x00;  // Opcode (2 for WRQ)
    buffer[wrq_len++] = 0x02;
    strcpy(&buffer[wrq_len], filename);  // Filename
    wrq_len += strlen(filename) + 1;    // +1 for null terminator
    strcpy(&buffer[wrq_len], "octet");  // Mode (binary transfer)
    wrq_len += strlen("octet") + 1;     // +1 for null terminator

    // Send the WRQ packet to the server
    if (send(sockfd, buffer, wrq_len, 0) < 0) {
        perror("Failed to send WRQ");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("WRQ sent for file: %s\n", filename);

    // Close the socket
    close(sockfd);
    return EXIT_SUCCESS;
}

