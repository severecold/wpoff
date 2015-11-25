
#ifndef _MMC_CMDS_H_
#define _MMC_CMDS_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mmc_write_prot_type {
    temp_wp,
    power_on_wp,
    perm_wp,
    invalid_wp,
} mmc_write_prot_type;

int mmc_read_extcsd(int fd, __u8 *extcsd);
int mmc_write_extcsd_byte(int fd, __u8 index, __u8 value);
int mmc_set_extcsd_bits(int fd, __u8 index, __u8 value);
int mmc_clr_extcsd_bits(int fd, __u8 index, __u8 value);
//
int mmc_send_rca(int fd, __u32 *rca);
//
int mmc_send_status(int fd, __u32 rca, __u32 *s);
// set USER_WP bits
int mmc_set_user_wp(int fd, mmc_write_prot_type type);
// clear USER_WP bits
int mmc_clr_user_wp(int fd);
// get card size
int mmc_get_card_sz(__u8 *extcsd, __u32 *size);
// get wp group size
int mmc_get_wp_grp_sz(__u8 *extcsd, __u32 *size);
// get USER_WP
int mmc_get_user_wp(__u8 *extcsd, __u8 *wp);
// read status of 32 groups
int mmc_send_write_prot_type(int fd, __u32 address, __u64 *result);
// set 1 group
int mmc_set_write_prot(int fd, __u32 address);
// clear 1 group
int mmc_clr_write_prot(int fd, __u32 address);
//
int mmc_set_class_6_ctrl(int fd, __u8 byte);
//
int mmc_gen_cmd(int fd, __u32 arg, __u8 *data);

#ifdef __cplusplus
}
#endif

#endif

