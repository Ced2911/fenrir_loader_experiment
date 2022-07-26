CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
SET(CMAKE_SYSTEM_NAME Yaul)
SET(CMAKE_SYSTEM_VERSION 1)

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("$ENV{YAUL_INSTALL_ROOT}bin/$ENV{YAUL_ARCH_SH_PREFIX}-gcc" GNU)
CMAKE_FORCE_CXX_COMPILER("$ENV{YAUL_INSTALL_ROOT}bin/$ENV{YAUL_ARCH_SH_PREFIX}-g++" GNU)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -m2 -mb -specs=$ENV{YAUL_INSTALL_ROOT}/$ENV{YAUL_ARCH_SH_PREFIX}/lib/yaul-main.specs -specs=$ENV{YAUL_INSTALL_ROOT}/$ENV{YAUL_ARCH_SH_PREFIX}/lib/yaul.specs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -m2 -mb -specs=$ENV{YAUL_INSTALL_ROOT}/$ENV{YAUL_ARCH_SH_PREFIX}/lib/yaul-main.specs -specs=$ENV{YAUL_INSTALL_ROOT}/$ENV{YAUL_ARCH_SH_PREFIX}/lib/yaul.specs")
SET(CMAKE_FIND_ROOT_PATH "$ENV{YAUL_INSTALL_ROOT}bin")



set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(YAUL_INCLUDE_DIRS 
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/bcl
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/tga
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/sega3d
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/common
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/dbgio
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/math
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/fs/fileclient
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/fs/romdisk
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/a/cs0/dram-cart
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/a/cs0/usb-cart
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/a/cs2/cd-block
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/b/scsp
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/b/vdp
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/cpu
    $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/scu/bus/cpu/smpc
)

set(YAUL_SYSTEM_INCLUDE_DIRS
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/include
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/include-fixed
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/../../../../$ENV{YAUL_ARCH_SH_PREFIX}/sys-include
    $ENV{YAUL_INSTALL_ROOT}bin/../lib/gcc/$ENV{YAUL_ARCH_SH_PREFIX}/8.4.0/../../../../$ENV{YAUL_ARCH_SH_PREFIX}/include
)

include_directories($ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/include/yaul/)
include_directories(${YAUL_INCLUDE_DIRS})
include_directories(SYSTEM ${YAUL_SYSTEM_INCLUDE_DIRS})

PROJECT(FenrirYaul)

set(FENRIR_SRC src/main.c)

add_executable(${PROJECT_NAME} ${FENRIR_SRC})
target_link_libraries(${PROJECT_NAME} $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/lib/libyaul.a $ENV{YAUL_INSTALL_ROOT}$ENV{YAUL_ARCH_SH_PREFIX}/lib/crt0.o )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--defsym=__master_stack=0x06004000 -Wl,--defsym=__slave_stack=0x06001000")
