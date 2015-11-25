
#include "mmc_part.h"
#include "utils.h"
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static int mmc_find_part(const char *name, struct mmc_part_info *info) {
    int ret = -1, rc;
    char path_by_name[PATH_MAX];
    DIR *dir;
    struct dirent *d;

    dir = opendir("/dev/block/platform");
    if (!dir)
        return ret;
    while ((d = readdir(dir)) != NULL) {
        char path_abs[PATH_MAX];
        char *test;

        if (d->d_name[0] == '.')
            continue;
        snprintf(path_by_name, sizeof(path_by_name),
            "/dev/block/platform/%s/by-name/%s", d->d_name, name);
        memset(path_abs, 0, sizeof(path_abs));
        rc = readlink(path_by_name, path_abs, sizeof(path_abs) - 1);
        if (rc <= 0)
            continue;
        test = strstr(path_abs, "mmcblk");
        if (test) {
            info->blk = test[6] - '0';
            info->part = atoi(test + 8);
            strncpy(info->path, path_abs, sizeof(info->path));
            ret = 0;
            break;
        }
    }
    closedir(dir);

    return ret;
}

int mmc_part_info_get(const char *name, struct mmc_part_info *info) {
    int rc;
    DIR *dir;
    struct dirent *d;
    char device_base[PATH_MAX];
    char device_offset[PATH_MAX];
    char device_size[PATH_MAX];
    uint64_t val;

    rc = mmc_find_part(name, info);
    if (rc < 0)
        return -1;
    dir = opendir("/sys/bus/mmc/devices");
    if (!dir)
        return -1;
    rc = -1;
    while ((d = readdir(dir)) != NULL) {
        struct stat fs;

        if (d->d_name[0] == '.')
            continue;
        snprintf(device_base, sizeof(device_base),
                 "/sys/bus/mmc/devices/%s/block/mmcblk%d/mmcblk%dp%d",
                 d->d_name, info->blk, info->blk, info->part);
        rc = stat(device_base, &fs);
        if (!rc)
            break;
    }
    closedir(dir);
    if (rc)
        return -1;
    snprintf(device_offset, sizeof(device_offset),
             "%s/start", device_base);
    rc = file_read_uint(device_offset, &val, 10);
    if (rc < 0)
        return -1;
    info->offset = (uint32_t) val;
    snprintf(device_size, sizeof(device_size),
             "%s/size", device_base);
    rc = file_read_uint(device_size, &val, 10);
    if (rc < 0)
        return -1;
    info->size = (uint32_t) val;

    return 0;
}

