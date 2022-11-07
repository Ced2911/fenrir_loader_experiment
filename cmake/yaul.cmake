
# common for yaul projects
SET(CMAKE_SYSTEM_NAME Yaul)
SET(CMAKE_SYSTEM_VERSION 1)

include(CMakeForceCompiler)

add_link_options(-Wl,--gc-sections --specs=${CMAKE_SOURCE_DIR}/libyaul.specs -ffreestanding -m2 -mb)
add_link_options(-Wl,--defsym=___master_stack=${IP_MASTER_STACK_ADDR} -Wl,--defsym=___slave_stack=${IP_SLAVE_STACK_ADDR})
add_compile_options(--specs=${CMAKE_SOURCE_DIR}/libyaul.specs -ffunction-sections -fdata-sections  -ffreestanding -m2 -mb)

CMAKE_FORCE_C_COMPILER("$ENV{YAUL_INSTALL_ROOT}/bin/$ENV{YAUL_ARCH_SH_PREFIX}-gcc" GNU)
CMAKE_FORCE_CXX_COMPILER("$ENV{YAUL_INSTALL_ROOT}/bin/$ENV{YAUL_ARCH_SH_PREFIX}-g++" GNU)
SET(CMAKE_FIND_ROOT_PATH "$ENV{YAUL_INSTALL_ROOT}bin")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(YAUL_INCDIR         $ENV{YAUL_INSTALL_ROOT}/$ENV{YAUL_ARCH_SH_PREFIX}/include/)
set(YAUL_INSTALL_ROOT   $ENV{YAUL_INSTALL_ROOT}/$ENV{YAUL_ARCH_SH_PREFIX})

set(YAUL_LIBS   )

set(YAUL_INCLUDE_DIRS 
    ${YAUL_INCDIR}/bcl
    ${YAUL_INCDIR}/tga
    ${YAUL_INCDIR}/sega3d
    ${YAUL_INCDIR}/yaul
    ${YAUL_INCDIR}/yaul/common
    ${YAUL_INCDIR}/yaul/dbgio
    ${YAUL_INCDIR}/yaul/math
    ${YAUL_INCDIR}/yaul/fs/fileclient
    ${YAUL_INCDIR}/yaul/fs/romdisk
    ${YAUL_INCDIR}/yaul/scu
    ${YAUL_INCDIR}/yaul/scu/bus/a/cs0/dram-cart
    ${YAUL_INCDIR}/yaul/scu/bus/a/cs0/usb-cart
    ${YAUL_INCDIR}/yaul/scu/bus/a/cs2/cd-block
    ${YAUL_INCDIR}/yaul/scu/bus/b/scsp
    ${YAUL_INCDIR}/yaul/scu/bus/b/vdp
    ${YAUL_INCDIR}/yaul/scu/bus/cpu
    ${YAUL_INCDIR}/yaul/scu/bus/cpu/smpc
)

set(YAUL_SYSTEM_INCLUDE_DIRS
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/include
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/include-fixed
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/../../../../$ENV{YAUL_ARCH_SH_PREFIX}/sys-include
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/../../../../$ENV{YAUL_ARCH_SH_PREFIX}/include
)

include_directories(${YAUL_INCDIR}/yaul/)
include_directories(${YAUL_INCLUDE_DIRS})
include_directories(SYSTEM ${YAUL_SYSTEM_INCLUDE_DIRS})