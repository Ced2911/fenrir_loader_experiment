# libmikmod Makefile for targetting Sega Saturn (m68k)

CFLAGS = -std=gnu99 -O2 -g -Wall -I$(M68K_INST)/include 
CFLAGS+= -I$(CURDIR)/../include/ 
CFLAGS+= -I$(CURDIR)
CFLAGS+= -DDRV_SCSP_M68K -DHAVE_LIMITS_H -DHAVE_SNPRINTF
ASFLAGS = 
LDFLAGS = -nostartfiles -nostdlib
M68KPREFIX = m68k-linux-gnu-
INSTALLDIR = $(M68K_INST)
CC = $(M68KPREFIX)gcc-13
AS = $(M68KPREFIX)as
LD = $(M68KPREFIX)ld
AR = $(M68KPREFIX)ar

all: libmikmod.a

SRCS =	../drivers/drv_scsp_m68k.c	\
	../drivers/drv_nos.c	\
	\
	../loaders/load_669.c	\
	../loaders/load_amf.c	\
	../loaders/load_asy.c	\
	../loaders/load_dsm.c	\
	../loaders/load_far.c	\
	../loaders/load_gdm.c	\
	../loaders/load_gt2.c	\
	../loaders/load_it.c	\
	../loaders/load_imf.c	\
	../loaders/load_m15.c	\
	../loaders/load_med.c	\
	../loaders/load_mod.c	\
	../loaders/load_mtm.c	\
	../loaders/load_okt.c	\
	../loaders/load_s3m.c	\
	../loaders/load_stm.c	\
	../loaders/load_stx.c	\
	../loaders/load_ult.c	\
	../loaders/load_umx.c	\
	../loaders/load_uni.c	\
	../loaders/load_xm.c	\
	\
	../depackers/mmcmp.c	\
	../depackers/pp20.c	\
	../depackers/s404.c	\
	../depackers/xpk.c	\
	\
	../mmio/mmalloc.c	\
	../mmio/mmerror.c	\
	../mmio/mmio.c		\
	../posix/strcasecmp.c	\
	\
	../playercode/mdreg.c	\
	../playercode/mdriver.c	\
	../playercode/mloader.c	\
	../playercode/mlreg.c	\
	../playercode/mlutil.c	\
	../playercode/mplayer.c	\
	../playercode/munitrk.c	\
	../playercode/mwav.c	\
	../playercode/npertab.c	\
	../playercode/sloader.c	\
	../playercode/virtch.c	\
	../playercode/virtch2.c	\
	../playercode/virtch_common.c
#	../playercode/mdulaw.c


DRV_SRCS =	driver.c

OBJS = $(SRCS:.c=.o)

DRV_OBJS = $(DRV_SRCS:.c=.o)

libmikmod.a: $(OBJS)
	$(AR) -rcs -o libmikmod.a $(OBJS)


driver.bin: driver.elf
	$(M68KPREFIX)objcopy  --output-format=binary $^ $@

driver.elf: libmikmod.a $(DRV_OBJS)
	$(CC) -g $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) *.a

all: libmikmod.a driver.bin