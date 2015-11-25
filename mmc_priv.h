
#ifndef _MMC_PRIV_H_
#define _MMC_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/* From kernel linux/major.h */
#define MMC_BLOCK_MAJOR         179

/* From kernel linux/mmc/core.h */
#define MMC_RSP_PRESENT  (1 << 0)
#define MMC_RSP_136      (1 << 1)       /* 136 bit response */
#define MMC_RSP_CRC      (1 << 2)       /* expect valid crc */
#define MMC_RSP_BUSY     (1 << 3)       /* card may send busy */
#define MMC_RSP_OPCODE   (1 << 4)       /* response contains opcode */

#define MMC_CMD_AC       (0 << 5)
#define MMC_CMD_ADTC     (1 << 5)
#define MMC_CMD_BCR      (3 << 5)

#define MMC_RSP_SPI_S1   (1 << 7)       /* one status byte */
#define MMC_RSP_SPI_BUSY (1 << 10)      /* card may send busy */

#define MMC_RSP_SPI_R1  (MMC_RSP_SPI_S1)
#define MMC_RSP_SPI_R1B (MMC_RSP_SPI_S1|MMC_RSP_SPI_BUSY)

#define MMC_RSP_R1  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R6  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

/* From kernel linux/mmc/mmc.h */
#define MMC_SWITCH_MODE_CMD_SET     0x00    /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS    0x01    /* Set bits which are 1 in value */
#define MMC_SWITCH_MODE_CLEAR_BITS  0x02    /* Clear bits which are 1 in value */
#define MMC_SWITCH_MODE_WRITE_BYTE  0x03    /* Set target to value */

/* */
#define MMC_SWITCH                  6    /* ac   [31:0] See below   R1b */
#define MMC_SEND_EXT_CSD            8    /* adtc                    R1  */
#define MMC_SEND_CSD                9    /* ac   [31:16] RCA        R2  */
#define MMC_SEND_STATUS             13
#define MMC_SET_WRITE_PROT          28   /* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT          29   /* ac   [31:0] data addr   R1b */
#define MMC_SEND_WRITE_PROT_TYPE    31   /* adtc [31:0] data addr   R1  */
#define MMC_GEN_CMD                 56   /* adtc [0] RD/WR          R1  */

/* */
#define EXT_CSD_CLASS_6_CTRL        59
#define EXT_CSD_USER_WP             171
#define EXT_CSD_ERASE_GROUP_DEF     175
#define EXT_CSD_SEC_CNT             212
#define HC_WP_GRP_SIZE              221
#define HC_ERASE_GRP_SIZE           224

/* */
#define EXT_CSD_CMD_SET_NORMAL      (1<<0)
#define USER_WP_US_PWR_WP_EN        (1<<0)
#define USER_WP_US_PERM_WP_EN       (1<<2)

/* From kernel linux/mmc/sd.h */
#define SD_SEND_RELATIVE_ADDR       3   /* bcr                     R6  */

#ifdef __cplusplus
}
#endif

#endif

