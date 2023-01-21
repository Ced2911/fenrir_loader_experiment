FROM archlinux:base-devel-20221218.0.111778

ENV YAUL_INSTALL_ROOT=/opt/tool-chains/sh2eb-elf/
ENV YAUL_PROG_SH_PREFIX=
ENV YAUL_ARCH_SH_PREFIX=sh2eb-elf
ENV YAUL_ARCH_M68K_PREFIX=m68keb-elf
ENV YAUL_BUILD_ROOT=${HOME}/libyaul
ENV YAUL_BUILD=build
ENV YAUL_CDB=1
ENV YAUL_OPTION_DEV_CARTRIDGE=0
ENV YAUL_OPTION_MALLOC_IMPL="tlsf"
ENV YAUL_OPTION_SPIN_ON_ABORT=1
ENV YAUL_OPTION_BUILD_GDB=0
ENV YAUL_OPTION_BUILD_ASSERT=0
#ENV SILENT=1
#ENV NOCOLOR=
ENV MAKE_ISO_XORRISO=/usr/bin/xorriso
ENV CDB_GCC=/usr/bin/gcc
ENV CDB_CPP=/usr/bin/g++

# install required tools
RUN echo -e "[yaul-linux]\nSigLevel = Optional TrustAll\nServer = http://packages.yaul.org/linux/x86_64\n" | sudo tee -a /etc/pacman.conf && \
    pacman -Syy && \
    pacman -S --noconfirm git yaul-tool-chain-git yaul yaul-examples-git vim cmake nodejs npm yarn icu rust rustup \
    rustup default stable
# yaul-examples-git 



