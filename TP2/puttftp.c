#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define TFTP_PORT 69
#define BUFFER_SIZE 516  // TFTP packets are max 516 bytes
#define DATA_SIZE 512    // Max data size for a TFTP data packet

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

    // Store server address for use in sendto and recvfrom
    struct sockaddr_storage server_addr;
    socklen_t server_addr_len = res->ai_addrlen;
    memcpy(&server_addr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

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
    if (sendto(sockfd, buffer, wrq_len, 0, (struct sockaddr *)&server_addr, server_addr_len) < 0) {
        perror("Failed to send WRQ");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("WRQ sent for file: %s\n", filename);

    // Wait for ACK for WRQ
    int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
    if (recv_len < 0) {
        perror("Failed to receive ACK for WRQ");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Validate the ACK
    if (buffer[1] != 0x04 || buffer[2] != 0x00 || buffer[3] != 0x00) {
        fprintf(stderr, "Invalid ACK received for WRQ\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Open the file to read data
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Send the file data in TFTP DATA packets
    int block_num = 1;
    while (1) {
        int data_len = fread(buffer + 4, 1, DATA_SIZE, file);  // Read up to 512 bytes
        if (data_len < 0) {
            perror("Failed to read file");
            break;
        }

        // Construct the DATA packet
        buffer[0] = 0x00;  // Opcode (3 for DATA)
        buffer[1] = 0x03;
        buffer[2] = (block_num >> 8) & 0xFF;  // Block number (high byte)
        buffer[3] = block_num & 0xFF;         // Block number (low byte)

        // Send the DATA packet
        if (sendto(sockfd, buffer, data_len + 4, 0, (struct sockaddr *)&server_addr, server_addr_len) < 0) {
            perror("Failed to send DATA packet");
            break;
        }

        // Wait for ACK
        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
        if (recv_len < 0) {
            perror("Failed to receive ACK");
            break;
        }

        // Validate the ACK
        if (buffer[1] != 0x04 || buffer[2] != ((block_num >> 8) & 0xFF) || buffer[3] != (block_num & 0xFF)) {
            fprintf(stderr, "Invalid ACK received for block %d\n", block_num);
            break;
        }

        printf("ACK received for block %d\n", block_num);

        block_num++;

        // If the last packet was smaller than 512 bytes, we're done
        if (data_len < DATA_SIZE) break;
    }

    fclose(file);
    close(sockfd);
    printf("File %s uploaded successfully.\n", filename);

    return EXIT_SUCCESS;
}

