
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/mmc/ioctl.h>
#include "mmc_priv.h"
#include "mmc_cmds.h"

int mmc_read_extcsd(int fd, __u8 *extcsd) {
	int ret = 0;
	struct mmc_ioc_cmd idata;
	memset(&idata, 0, sizeof(idata));
	memset(extcsd, 0, sizeof(__u8) * 512);
	idata.write_flag = 0;
	idata.opcode = MMC_SEND_EXT_CSD;
	idata.arg = 0;
	idata.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
	idata.blksz = 512;
	idata.blocks = 1;
	mmc_ioc_cmd_set_data(idata, extcsd);

	ret = ioctl(fd, MMC_IOC_CMD, &idata);

	return ret;
}

static int mmc_write_extcsd_value(int fd, __u8 index, __u8 value, __u8 mode) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    idata.write_flag = 1;
    idata.opcode = MMC_SWITCH;
    idata.arg = (mode << 24) |
            (index << 16) |
            (value << 8) |
            EXT_CSD_CMD_SET_NORMAL;
    idata.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;

    ret = ioctl(fd, MMC_IOC_CMD, &idata);

	return ret;
}

int mmc_write_extcsd_byte(int fd, __u8 index, __u8 value) {
    return mmc_write_extcsd_value(fd, index, value, MMC_SWITCH_MODE_WRITE_BYTE);
}

int mmc_set_extcsd_bits(int fd, __u8 index, __u8 value) {
    return mmc_write_extcsd_value(fd, index, value, MMC_SWITCH_MODE_SET_BITS);
}

int mmc_clr_extcsd_bits(int fd, __u8 index, __u8 value) {
    return mmc_write_extcsd_value(fd, index, value, MMC_SWITCH_MODE_CLEAR_BITS);
}

int mmc_send_rca(int fd, __u32 *rca) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    idata.opcode = SD_SEND_RELATIVE_ADDR;
    idata.arg = 0;
    idata.flags = MMC_RSP_R6 | MMC_CMD_BCR;

    ret = ioctl(fd, MMC_IOC_CMD, &idata);
    if (!ret)
        *rca = idata.response[0] >> 16;

    return ret;

}

int mmc_send_status(int fd, __u32 rca, __u32 *s) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    idata.opcode = MMC_SEND_STATUS;
    idata.arg = rca << 16;
    idata.flags = MMC_RSP_R1 | MMC_CMD_AC;

    ret = ioctl(fd, MMC_IOC_CMD, &idata);
    if (!ret)
        *s = idata.response[0];

    return ret;
}

int mmc_set_user_wp(int fd, mmc_write_prot_type type) {
    int rc;
    __u8 extcsd[512];
    __u8 user_wp;

    rc = mmc_read_extcsd(fd, extcsd);
    if (rc)
        return rc;
    user_wp = extcsd[EXT_CSD_USER_WP];
    switch (type) {
    case temp_wp: {
        rc = mmc_clr_extcsd_bits(fd,
            EXT_CSD_USER_WP,
            USER_WP_US_PWR_WP_EN | USER_WP_US_PERM_WP_EN);
        break;
    }
    case power_on_wp: {
        user_wp &= ~(USER_WP_US_PERM_WP_EN);
        user_wp |= USER_WP_US_PWR_WP_EN;
        rc = mmc_write_extcsd_byte(fd,
            EXT_CSD_USER_WP,
            user_wp);
    }
    case perm_wp: {
        rc = mmc_set_extcsd_bits(fd,
            EXT_CSD_USER_WP,
            USER_WP_US_PERM_WP_EN);
        break;
    }
    default: {
        rc = -1;
        break;
    }
    }

    return rc;
}

int mmc_clr_user_wp(int fd) {
    int rc;
    __u8 extcsd[512];
    __u8 user_wp;

    rc = mmc_read_extcsd(fd, extcsd);
    if (rc)
        return rc;
    user_wp = extcsd[EXT_CSD_USER_WP];
    user_wp &= ~(USER_WP_US_PWR_WP_EN | USER_WP_US_PERM_WP_EN);
    return mmc_clr_extcsd_bits(fd, EXT_CSD_USER_WP, user_wp);
}

// in sectors
int mmc_get_card_sz(__u8 *extcsd, __u32 *size) {
    int ret = 0;

    *size = extcsd[EXT_CSD_SEC_CNT] |
            extcsd[EXT_CSD_SEC_CNT + 1] << 8 |
            extcsd[EXT_CSD_SEC_CNT + 2] << 16 |
            extcsd[EXT_CSD_SEC_CNT + 3] << 24;

    return ret;
}

// in sectors
int mmc_get_wp_grp_sz(__u8 *extcsd, __u32 *size) {
    int ret = 0;

   /*
    *
    * HC_ERASE_GRP_SIZE *
    * HC_WP_GRP_SIZE *
    * 512KB
    *
    */
   if (extcsd[EXT_CSD_ERASE_GROUP_DEF]) {
        *size = extcsd[HC_ERASE_GRP_SIZE] * extcsd[HC_WP_GRP_SIZE] * 1024;
   } else {
        // old style, not supported
        *size = 0;
        ret = -1;
   }

   return ret;
}

int mmc_get_user_wp(__u8 *extcsd, __u8 *wp) {
    *wp = extcsd[EXT_CSD_USER_WP];
    return 0;
}

int mmc_send_write_prot_type(int fd, __u32 address, __u64 *result) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    memset(result, 0, sizeof(*result));
    idata.opcode = MMC_SEND_WRITE_PROT_TYPE;
    idata.arg = address;
    idata.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata.blksz = sizeof(*result);
    idata.blocks = 1;
    mmc_ioc_cmd_set_data(idata, result);

    ret = ioctl(fd, MMC_IOC_CMD, &idata);

    return ret;
}

int mmc_set_write_prot(int fd, __u32 address) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    idata.write_flag = 1;
    idata.opcode = MMC_SET_WRITE_PROT;
    idata.arg = address;
    idata.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;

	ret = ioctl(fd, MMC_IOC_CMD, &idata);

	return ret;
}

int mmc_clr_write_prot(int fd, __u32 address) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    idata.write_flag = 1;
    idata.opcode = MMC_CLR_WRITE_PROT;
    idata.arg = address;
    idata.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;

	ret = ioctl(fd, MMC_IOC_CMD, &idata);

	return ret;
}

int mmc_set_class_6_ctrl(int fd, __u8 byte) {
    return mmc_write_extcsd_byte(fd, EXT_CSD_CLASS_6_CTRL, byte);
}

int mmc_gen_cmd(int fd, __u32 arg, __u8 *data) {
    int ret = 0;
    struct mmc_ioc_cmd idata;

    memset(&idata, 0, sizeof(idata));
    idata.write_flag = (arg & 1) ? 0 : 1;
    idata.opcode = MMC_GEN_CMD;
    idata.arg = arg;
    idata.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
    idata.blksz = 512;
    idata.blocks = 1;
    mmc_ioc_cmd_set_data(idata, data);

    ret = ioctl(fd, MMC_IOC_CMD, &idata);

	return ret;
}

