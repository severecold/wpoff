
#include "mmc_sh.h"
#include "mmc_cmds.h"
#include "android.h"
#include <string.h>

struct sh_gen_cmd_p {
    const char *model;
    const __u8 *param;
};

#ifdef HAVE_306SH
static const __u8 gc_306sh[0x28] = {
    0x01, 0x0D, 0x34, 0x44, 0x0D, 0x49, 0x1C, 0xF0, 0xD4, 0xE2, 0x4D, 0x36, 0x5E,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81,
};
#endif

static const struct sh_gen_cmd_p s_known_gcs[] = {
#ifdef HAVE_306SH
    {
        .model = "306SH",
        .param = gc_306sh,
    },
#endif
    {
        .model = NULL,
    },
};

static int get_gen_cmd(__u8 *data) {
    char val[PROPERTY_VALUE_MAX];
    const struct sh_gen_cmd_p *p;
    const __u8 *src;
    __u8 *dst;

    memset(val, 0, sizeof(val));
    property_get("ro.product.model", val, "");
    for (p = &s_known_gcs[0]; p->model != NULL; p++) {
        if (!strcmp(p->model, val))
            break;
    }
    if (p->model == NULL)
        return -1;
    src = p->param;
    dst = data;
    memset(dst, 0, 0x28);
    while (*src) {
        __u8 i, off, cnt;

        off = *src >> 4;
        cnt = *src & 0x0F;
        src++;
        dst += off;
        for (i = 0; i < cnt; i++) {
            *dst++ = *src++;
        }
    }

    return 0;
}

int mmc_sh_pre_clr_wp(int fd) {
    int rc;
    __u8 data[512];
    __u8 op;

    rc = get_gen_cmd(data);
    if (rc < 0)
        return -1;
    op = 2;
    data[0x20] = op;
    // write
    rc = mmc_gen_cmd(fd, 0, data);
    if (rc) {
        // printf("gen cmd 1\n");
        return -1;
    }
    // read
    rc = mmc_gen_cmd(fd, 1, data);
    if (rc) {
        // printf("gen cmd 2\n");
        return -1;
    }
    rc = get_gen_cmd(data);
    if (rc < 0)
        return -1;
    op = 1;
    data[0x20] = op;
    // write
    rc = mmc_gen_cmd(fd, 0, data);
    if (rc)
        return -1;
    // read
    rc = mmc_gen_cmd(fd, 1, data);
    if (rc)
        return -1;
    // dump("mmc_gen_cmd:", data, sizeof(data));

    return 0;
}


