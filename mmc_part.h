
#ifndef _MMC_PART_H_
#define _MMC_PART_H_

#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mmc_part_info {
    char path[PATH_MAX];
    int blk;
    int part;
    uint32_t offset;
    uint32_t size;
};

int mmc_part_info_get(const char *name, struct mmc_part_info *info);

#ifdef __cplusplus
}
#endif

#endif

