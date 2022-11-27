ifeq ($(strip $(YAUL_INSTALL_ROOT)),)
  $(error Undefined YAUL_INSTALL_ROOT (install root directory))
endif

include $(YAUL_INSTALL_ROOT)/share/build.pre.mk


BUILTIN_ASSETS+=

SH_PROGRAM:= fenrir-yaul

SH_SRCS:= \
	assets/army12.c \
	assets/bitmap.c \
	src/compat.c \
	src/main.c \
	src/noise.c \
	src/reset.c \
	src/vdp1.config.c \
	src/vdp2.config.c \
	src/font/font.c \
	src/fenrir/fenrir.c \
	src/fenrir/fenrir.dir_op.c \
	src/fenrir/fenrir.asserts.c \
	src/fenrir/region.c \
	src/screens/gamelist.c \
	src/screens/gamelist_theme.c \
	src/screens/browser.c \
	src/screens/error.c \
	src/screens/screen.c \
	src/theme.c \
	src/sound_driver/pcm.c \
	src/lz4/lz4.c
	
SH_LIBRARIES:=
SH_CFLAGS+= -O2 -I. -save-temps=obj 
SH_CFLAGS+=-Isrc -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable  

IP_VERSION:= V0.001
IP_RELEASE_DATE:= 20210301
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKST
IP_TITLE:= FENRIR-LOADER
IP_MASTER_STACK_ADDR:= 0x06100000
IP_SLAVE_STACK_ADDR:= 0x06002000
IP_1ST_READ_ADDR:= 0x06004000
IP_1ST_READ_SIZE:= 0



$(SH_PROGRAM)-cd.bin: $(SH_PROGRAM).iso
	tools/iso2bin/iso2bin $(SH_PROGRAM).iso $(SH_PROGRAM)-cd.bin

.build: $(SH_PROGRAM)-cd.bin

include $(YAUL_INSTALL_ROOT)/share/build.post.iso-cue.mk
