
CC := arm-linux-androideabi-gcc
LD := arm-linux-androideabi-ld

CFLAGS := -Wall -Os -DHAVE_306SH
LDFLAGS := -static -lc

all: wpoff

common_OBJS := android.o \
               utils.o \
               sharp.o

mmc_OBJS := mmc_cmds.o \
            mmc_write_prot.o \
            mmc_sh.o \
            mmc_part.o

wpoff_OBJS := main.o
wpoff_OBJS += $(common_OBJS)
wpoff_OBJS += $(mmc_OBJS)

wpoff: $(wpoff_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $^

clean:
	rm -f wpoff
	rm -f $(wpoff_OBJS)

