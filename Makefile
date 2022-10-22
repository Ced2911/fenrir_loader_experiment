ifeq ($(strip $(YAUL_INSTALL_ROOT)),)
  $(error Undefined YAUL_INSTALL_ROOT (install root directory))
endif

include $(YAUL_INSTALL_ROOT)/share/pre.common.mk
#include libyaul_custom/makefiles/pre.common.mk

SH_PROGRAM:= fenrir-yaul
SH_SRCS:= \
	assets/army12.c \
	assets/bitmap.c \
	src/main.c \
	src/noise.c \
	src/reset.c \
	src/vdp1.config.c \
	src/vdp2.config.c \
	src/ui.config.c \
	src/font/font.c \
	src/fenrir/fenrir.c \
	src/fenrir/fenrir.dir_op.c \
	src/fenrir/fenrir.asserts.c \
	src/fenrir/region.c \
	src/screens/gamelist.c \
	src/screens/gamelist_theme.c \
	src/screens/browser.c \
	src/sound_driver/pcmsys_y.c
	
SH_LIBRARIES:=
SH_CFLAGS+= -O2 -I. -save-temps=obj 

IP_VERSION:= V0.001
IP_RELEASE_DATE:= 20210301
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKST
IP_TITLE:= FENRIR-LOADER
IP_MASTER_STACK_ADDR:= 0x06100000
IP_SLAVE_STACK_ADDR:= 0x06002000
IP_1ST_READ_ADDR:= 0x06004000

M68K_PROGRAM:=
M68K_OBJECTS:=
SH_CFLAGS+=-Isrc -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable 



# Add new inits code...
#SH_SRCS += libyaul_custom/crt0.sx
#SH_LDFLAGS+=-T libyaul_custom/ldscripts/yaul.x
#SH_CUSTOM_SPECS=libyaul_custom/specs/yaul-main.specs

include $(YAUL_INSTALL_ROOT)/share/post.common.mk 
#include libyaul_custom/makefiles/post.common.mk
