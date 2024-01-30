#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 8000

void simulate_client(const char *data) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_HOST, &server_addr.sin_addr) <= 0) {
        perror("inet_pton() error");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    // Send data to the server
    if (write(sockfd, data, strlen(data)) == -1) {
        perror("write() error");
    }

    // Optionally, you can read data from the server if needed
    // char buffer[1024];
    // ssize_t bytesRead = read(sockfd, buffer, sizeof(buffer));
    // if (bytesRead == -1) {
    //     perror("read() error");
    // } else {
    //     printf("Received from server: %.*s\n", (int)bytesRead, buffer);
    // }

    close(sockfd);
}

int main() {
    // Simulate multiple clients
    for (int i = 0; i < 5; ++i) {
        char data_to_send[256];
        snprintf(data_to_send, sizeof(data_to_send), "Client %d: Hello from client %d!", i + 1, i + 1);
        simulate_client(data_to_send);
        sleep(1);  // Simulate some processing time
    }

    return 0;
}
