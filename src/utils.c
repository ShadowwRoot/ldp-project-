#include "utils.h"
#include <stdio.h>
#include <string.h>

void log_credentials(const char *data) {
    FILE *file = fopen("credentials.log", "a");
    if (!file) {
        perror("[UTILS] fopen");
        return;
    }
    fprintf(file, "%s\n", data);
    fclose(file);
}
