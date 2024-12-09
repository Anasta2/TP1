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
    if (send(sockfd, buffer, rrq_len, 0) < 0) {
        perror("Failed to send RRQ");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("RRQ sent for file: %s\n", filename);

    // Open a file to write the received data
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Receive a single DATA packet
    int recv_len = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (recv_len < 0) {
        perror("Failed to receive DATA packet");
        fclose(file);
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Validate the DATA packet
    if (buffer[1] != 0x03) {  // Opcode (3 for DATA)
        fprintf(stderr, "Unexpected packet received\n");
        fclose(file);
        close(sockfd);
        return EXIT_FAILURE;
    }

    int block_num = (buffer[2] << 8) | buffer[3];
    printf("Received DATA packet, block: %d, size: %d bytes\n", block_num, recv_len - 4);

    // Write data to the file
    fwrite(buffer + 4, 1, recv_len - 4, file);

    // Send an ACK for the received DATA packet
    char ack[4] = {0x00, 0x04, buffer[2], buffer[3]};
    if (send(sockfd, ack, sizeof(ack), 0) < 0) {
        perror("Failed to send ACK");
        fclose(file);
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("ACK sent for block: %d\n", block_num);

    fclose(file);
    close(sockfd);
    printf("File transfer complete for single packet.\n");

    return EXIT_SUCCESS;
}

