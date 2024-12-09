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
        fprintf(stderr, "Usage: %s <host> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *host = argv[1];
    const char *filename = argv[2];

    // Resolve the server address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_DGRAM;  // UDP socket

    if (getaddrinfo(host, NULL, &hints, &res) != 0) {
        perror("Failed to resolve host");
        return EXIT_FAILURE;
    }

    // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    // Configure server address
    struct sockaddr_in server_addr = *(struct sockaddr_in *)res->ai_addr;
    server_addr.sin_port = htons(TFTP_PORT);  // Set TFTP port
    freeaddrinfo(res);

    // Construct a TFTP Read Request (RRQ) packet
    char buffer[BUFFER_SIZE];
    int rrq_len = 0;

    buffer[rrq_len++] = 0x00;  // Opcode (1 for RRQ)
    buffer[rrq_len++] = 0x01;
    strcpy(&buffer[rrq_len], filename);  // Filename
    rrq_len += strlen(filename) + 1;    // +1 for null terminator
    strcpy(&buffer[rrq_len], "octet");  // Mode (binary transfer)
    rrq_len += strlen("octet") + 1;     // +1 for null terminator

    // Send the RRQ packet to the server
    if (sendto(sockfd, buffer, rrq_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to send RRQ");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("RRQ sent for file: %s\n", filename);

    // Receive the server's response
    socklen_t addr_len = sizeof(server_addr);
    int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (recv_len < 0) {
        perror("Failed to receive response");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Save the received data to a file
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        close(sockfd);
        return EXIT_FAILURE;
    }

    fwrite(buffer + 4, 1, recv_len - 4, file);  // Skip the 4-byte TFTP header
    fclose(file);

    printf("File %s downloaded successfully.\n", filename);

    close(sockfd);
    return EXIT_SUCCESS;
}

