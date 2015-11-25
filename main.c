
#include <stdio.h>
#include <string.h>
#include "mmc_write_prot.h"

static int usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s --dump\n", prog);
    fprintf(stderr, "%s --unprotect-all\n", prog);
    fprintf(stderr, "%s --unprotect <partition> ...\n", prog);
    fprintf(stderr, "%s --protect <partition> ...\n", prog);
    fprintf(stderr, "%s --protect-power-on <partition> ...\n", prog);
    fprintf(stderr, "%s --protect-permanent <partition> ...\n", prog);

    return 1;
}

int main(int argc, char *argv[]) {
    int rc, i;
    int on = 0;
    mmc_write_prot_type on_type = invalid_wp;

    if (argc < 2) {
        return usage(argv[0]);
    }

    if (!strcmp(argv[1], "--dump")) {
        if (argc != 2)
            return usage(argv[0]);
        rc = mmc_write_prot_dump();
        return rc;
    }
    if (!strcmp(argv[1], "--unprotect-all")) {
        if (argc != 2)
            return usage(argv[0]);
        rc = mmc_write_prot_off();
        return rc;
    }

    if (!strcmp(argv[1], "--protect")) {
        on = 1;
        on_type = temp_wp;
    } else if (!strcmp(argv[1], "--protect-power-on")) {
        on = 1;
        on_type = power_on_wp;
    } else if (!strcmp(argv[1], "--protect-permanent")) {
        on = 1;
        on_type = perm_wp;
    } else if (strcmp(argv[1], "--unprotect"))
        return usage(argv[0]);

    for (i = 2; i < argc; i++) {
        fprintf(stdout, "WP%s: %s ... ", on ? "ON" : "OFF", argv[i]);
        fflush(stdout);
        if (on)
            rc = mmc_write_prot_on_part(argv[i], on_type);
        else
            rc = mmc_write_prot_off_part(argv[i]);
        fprintf(stdout, "%s", rc ? "NG" : "OK");
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    return 0;
}

