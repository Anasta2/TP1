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
    hints.ai_family = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_DGRAM;  // UDP socket

    if (getaddrinfo(server, NULL, &hints, &res) != 0) {
        perror("Failed to resolve server address");
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
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        close(sockfd);
        return EXIT_FAILURE;
    }

    int block_num = 1;  // Expected block number
    while (1) {
        // Receive a TFTP DATA packet
        socklen_t addr_len = sizeof(server_addr);
        int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (recv_len < 0) {
            perror("Failed to receive DATA packet");
            break;
        }

        // Validate the DATA packet
        if (buffer[1] != 0x03) {  // Opcode (3 for DATA)
            fprintf(stderr, "Unexpected packet received\n");
            break;
        }

        int received_block = (buffer[2] << 8) | buffer[3];
        if (received_block != block_num) {
            fprintf(stderr, "Unexpected block number: %d\n", received_block);
            break;
        }

        // Write the received data to the file
        fwrite(buffer + 4, 1, recv_len - 4, file);

        // Send an ACK for the received DATA packet
        char ack[4] = {0x00, 0x04, buffer[2], buffer[3]};
        if (sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&server_addr, addr_len) < 0) {
            perror("Failed to send ACK");
            break;
        }

        printf("ACK sent for block %d\n", block_num);

        block_num++;

        // If the last packet was smaller than 512 bytes, we're done
        if (recv_len < BUFFER_SIZE) {
            printf("File transfer complete.\n");
            break;
        }
    }

    fclose(file);
    close(sockfd);
    return EXIT_SUCCESS;
}
