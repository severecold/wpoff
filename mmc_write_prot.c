
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "mmc_part.h"
#include "mmc_cmds.h"
#include "mmc_write_prot.h"
#include "mmc_sh.h"
#include "sharp.h"

static int mmc_get_dev(const char *part, char *device) {
    int rc;
    struct mmc_part_info info;
    char *test;

    rc = mmc_part_info_get(part, &info);
    if (rc < 0)
        return -1;
    strncpy(device, info.path, PATH_MAX);
    test = strstr(device, "mmcblk");
    if (test) {
        test += 6;
        while (isdigit(*test))
            test++;
        *test = '\0';
    }

    return 0;
}

int mmc_write_prot_dump() {
    int rc, fd;
    char device[PATH_MAX];
    __u8 extcsd[512];
    __u32 mmc_sz, wp_grp_sz, wp_grp_cnt, i;
    __u64 wp;

    rc = mmc_get_dev("system", device);
    if (rc < 0)
        return -1;
    fd = open(device, O_RDWR);
    if (fd < 0)
        return -1;
    rc = mmc_read_extcsd(fd, extcsd);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_get_card_sz(extcsd, &mmc_sz);
    if (rc) {
        close(fd);
        return -1;
    }
    printf("CARD SIZE: %08x\n", mmc_sz);
    rc = mmc_get_wp_grp_sz(extcsd, &wp_grp_sz);
    if (rc) {
        close(fd);
        return -1;
    }
    printf("WPGR SIZE: %08x\n", wp_grp_sz);
    wp_grp_cnt = mmc_sz / wp_grp_sz;
    for (i = 0; i < wp_grp_cnt; i += 32) {
        rc = mmc_send_write_prot_type(fd, i * wp_grp_sz, &wp);
        if (rc)
            break;
        printf(" %08x: %016llx\n", i * wp_grp_sz, wp);
    }
    close(fd);

    return i >= wp_grp_cnt ? 0 : -1;
}

int mmc_write_prot_off_part(const char *part) {
    int rc, fd;
    struct mmc_part_info info;
    char device[PATH_MAX], *test;
    __u8 extcsd[512];
    __u32 rca, wp_grp_sz;
    uint32_t i, wpst, wped;
    bool is_sharp = is_sharp_dev();

    rc = mmc_part_info_get(part, &info);
    if (rc < 0)
        return -1;
    strncpy(device, info.path, sizeof(device));
    test = strstr(device, "mmcblk");
    if (test) {
        test += 6;
        while (isdigit(*test))
            test++;
        *test = '\0';
    }
    fd = open(device, O_RDWR);
    if (fd < 0)
        return -1;
    rc = mmc_read_extcsd(fd, extcsd);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_get_wp_grp_sz(extcsd, &wp_grp_sz);
    if (rc) {
        close(fd);
        return -1;
    }
    wpst = info.offset & ~(wp_grp_sz - 1);
    wped = (info.offset + info.size) & ~(wp_grp_sz - 1);
    rc = mmc_send_rca(fd, &rca);
    if (rc) {
        // close(fd);
        // return -1;
        rca = 0x0001;
    }
    rc = mmc_set_class_6_ctrl(fd, 0);
    if (rc) {
        close(fd);
        return -1;
    }
    for (i = wpst; i < wped; i += wp_grp_sz) {
        __u32 status = 0;

        if (is_sharp && mmc_sh_pre_clr_wp(fd)) {
            // printf("failed mmc_sh_pre_clr_wp, %d\n", i);
            break;
        }
        // clear 1 group
        rc = mmc_clr_write_prot(fd, i);
        if (rc) {
            // printf("failed mmc_clr_write_prot, %d\n", i);
            break;
        }
        rc = mmc_send_status(fd, rca, &status);
        if (rc) {
            // printf("failed mmc_send_status, %d\n", i);
            break;
        }
    }
    close(fd);

    return i == wped ? 0 : -1;
}

int mmc_write_prot_on_part(const char *part, mmc_write_prot_type type) {
    int rc, fd;
    struct mmc_part_info info;
    char device[PATH_MAX], *test;
    __u8 extcsd[512];
    __u32 rca, wp_grp_sz;
    uint32_t i, wpst, wped;

    rc = mmc_part_info_get(part, &info);
    if (rc < 0)
        return -1;
    strncpy(device, info.path, sizeof(device));
    test = strstr(device, "mmcblk");
    if (test) {
        test += 6;
        while (isdigit(*test))
            test++;
        *test = '\0';
    }
    fd = open(device, O_RDWR);
    if (fd < 0)
        return -1;
    rc = mmc_read_extcsd(fd, extcsd);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_get_wp_grp_sz(extcsd, &wp_grp_sz);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_send_rca(fd, &rca);
    if (rc) {
        // close(fd);
        // return -1;
        rca = 0x0001;
    }
    rc = mmc_set_class_6_ctrl(fd, 0);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_set_user_wp(fd, type);
    if (rc) {
        close(fd);
        return -1;
    }
    wpst = info.offset & ~(wp_grp_sz - 1);
    wped = (info.offset + info.size) & ~(wp_grp_sz - 1);
    for (i = wpst; i < wped; i += wp_grp_sz) {
        // set 1 group
        // printf("%08x\n", i);
        rc = mmc_set_write_prot(fd, i);
        if (rc) {
            // printf("failed 2\n");
            break;
        }
    }
    mmc_clr_user_wp(fd);
    close(fd);

    return i == wped ? 0 : -1;
}

int mmc_write_prot_off() {
    int rc, fd;
    char device[PATH_MAX];
    __u8 extcsd[512];
    __u32 rca, card_sz, wp_grp_sz;
    __u32 i, loop;
    bool is_sharp = is_sharp_dev();

    rc = mmc_get_dev("system", device);
    if (rc)
        return -1;
    fd = open(device, O_RDWR);
    if (fd < 0)
        return -1;
    rc = mmc_read_extcsd(fd, extcsd);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_get_card_sz(extcsd, &card_sz);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_get_wp_grp_sz(extcsd, &wp_grp_sz);
    if (rc) {
        close(fd);
        return -1;
    }
    rc = mmc_send_rca(fd, &rca);
    if (rc) {
        // close(fd);
        // return -1;
        rca = 0x0001;
    }
    rc = mmc_set_class_6_ctrl(fd, 0);
    if (rc) {
        close(fd);
        return -1;
    }
    loop = card_sz / wp_grp_sz;
    for (i = 0; i < loop; i++) {
        __u32 status = 0;

        if (is_sharp && mmc_sh_pre_clr_wp(fd)) {
            // printf("failed mmc_sh_pre_clr_wp, %d\n", i * wp_grp_sz);
            break;
        }
        // clear 1 group
        rc = mmc_clr_write_prot(fd, i * wp_grp_sz);
        if (rc) {
            // printf("failed mmc_clr_write_prot, %d\n", i * wp_grp_sz);
            break;
        }
        rc = mmc_send_status(fd, rca, &status);
        if (rc) {
            // printf("failed mmc_send_status, %d\n", i * wp_grp_sz);
            break;
        }
    }
    close(fd);

    return i == loop ? 0 : -1;
}

