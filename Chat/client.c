#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_LENGTH 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip address> <port number> <command>\n", argv[0]);
        exit(1);
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Initialize server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("invalid address or address not supported");
        exit(1);
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(1);
    }

    // Send command to server
    char command[MAX_LENGTH];
    strcpy(command, argv[3]);
    ssize_t n = send(sockfd, command, strlen(command), 0);
    if (n < 0) {
        perror("error sending command");
        exit(1);
    }

    // Receive response from server
    char response[MAX_LENGTH];
    memset(response, 0, MAX_LENGTH);
    n = recv(sockfd, response, MAX_LENGTH, 0);
    if (n < 0) {
        perror("error receiving response");
        exit(1);
    }
    printf("%s", response);

    // Close socket
    close(sockfd);

    return 0;
}
