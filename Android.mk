
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := wpoff
LOCAL_SRC_FILES := main.c \
                   mmc_cmds.c \
                   mmc_part.c \
                   mmc_write_prot.c \
                   mmc_sh.c \
                   android.c \
                   sharp.c \
                   utils.c
LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += $(LLVM_CFLAGS)
LOCAL_CFLAGS += -I$(JNIROOT)/include -I$(JNIROOT)
LOCAL_CFLAGS += -DHAVE_SH04F
LOCAL_CFLAGS += -DHAVE_306SH
include $(BUILD_EXECUTABLE)

