
#ifndef _MMC_WRITE_PROT_H_
#define _MMC_WRITE_PROT_H_

#include "mmc_cmds.h"

#ifdef __cplusplus
extern "C" {
#endif

int mmc_write_prot_dump();
int mmc_write_prot_off_part(const char *part);
int mmc_write_prot_on_part(const char *part, mmc_write_prot_type type);
int mmc_write_prot_off();

#ifdef __cplusplus
}
#endif

#endif

