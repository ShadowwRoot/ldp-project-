#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 5555      // Port sur lequel le serveur écoute
#define MAX_CLIENTS 5  // Nombre maximum de connexions simultanées

// Gère les échanges avec un client connecté
void *client_handler(void *arg) {
    int client_sock = *(int*)arg;
    free(arg);  // Libère la mémoire allouée pour le socket

    char buffer[1024];
    ssize_t len;

    // Boucle pour recevoir et afficher les messages envoyés par le client
    while ((len = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[len] = '\0';
        printf("[C2] Received: %s\n", buffer);
    }

    // Vérifie si le client s'est déconnecté ou s'il y a une erreur
    if (len == 0) {
        printf("[C2] Client disconnected.\n");
    } else {
        perror("[C2] recv");
    }

    close(client_sock);  // Ferme la connexion avec le client
    pthread_exit(NULL);  // Termine le thread
}

int main() {
    int server_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Crée un socket pour le serveur
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[C2] socket");
        exit(1);
    }

    // Configure l'adresse et le port pour le serveur
    server_addr.sin_family = AF_INET;         // Utilisation d'IPv4
    server_addr.sin_port = htons(PORT);      // Définit le port
    server_addr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces réseau

    // Attache le socket à l'adresse et au port définis
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[C2] bind");
        close(server_sock);
        exit(1);
    }

    // Configure le socket pour écouter les connexions entrantes
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("[C2] listen");
        close(server_sock);
        exit(1);
    }

    printf("[C2] Server listening on port %d...\n", PORT);

    while (1) {
        // Accepte une connexion entrante
        addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("[C2] accept");
            continue;
        }

        printf("[C2] New connection accepted.\n");

        // Crée un thread pour gérer le client
        pthread_t thread_id;
        new_sock = malloc(sizeof(int));  // Alloue de la mémoire pour le socket du client
        *new_sock = client_sock;
        if (pthread_create(&thread_id, NULL, client_handler, (void*)new_sock) != 0) {
            perror("[C2] pthread_create");
            free(new_sock);  // Libère la mémoire si le thread ne peut pas être créé
        }
        pthread_detach(thread_id);  // Détache le thread pour qu'il puisse se nettoyer automatiquement
    }

    close(server_sock);  // Ferme le socket du serveur
    return 0;
}
