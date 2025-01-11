#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 5555
#define MAX_CLIENTS 5

void *client_handler(void *arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char buffer[1024];
    ssize_t len;

    while ((len = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[len] = '\0';
        printf("[C2] Received: %s\n", buffer);
    }

    if (len == 0) {
        printf("[C2] Client disconnected.\n");
    } else {
        perror("[C2] recv");
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main() {
    int server_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Créer le socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[C2] socket");
        exit(1);
    }

    // Configurer l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binder le socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[C2] bind");
        close(server_sock);
        exit(1);
    }

    // Écouter sur le socket
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("[C2] listen");
        close(server_sock);
        exit(1);
    }

    printf("[C2] Server listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("[C2] accept");
            continue;
        }

        printf("[C2] New connection accepted.\n");

        // Créer un thread pour gérer le client
        pthread_t thread_id;
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        if (pthread_create(&thread_id, NULL, client_handler, (void*)new_sock) != 0) {
            perror("[C2] pthread_create");
            free(new_sock);
        }
        pthread_detach(thread_id);
    }

    close(server_sock);
    return 0;
}
