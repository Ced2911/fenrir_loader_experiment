
# common for yaul projects
SET(CMAKE_SYSTEM_NAME Yaul)
SET(CMAKE_SYSTEM_VERSION 1)

include(CMakeForceCompiler)

set(YAUL_SPECS ${CMAKE_SOURCE_DIR}/cmake/libyaul.specs)

add_link_options(-Wl,--gc-sections --specs=${YAUL_SPECS} -ffreestanding -m2 -mb)
add_link_options(-Wl,--defsym=___master_stack=${IP_MASTER_STACK_ADDR} -Wl,--defsym=___slave_stack=${IP_SLAVE_STACK_ADDR})
add_compile_options(--specs=${YAUL_SPECS} -ffunction-sections -fdata-sections -ffreestanding -m2 -mb)

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


macro(gen_map target output)
    add_custom_command(TARGET ${target}
        POST_BUILD
        DEPENDS ${target}
        COMMAND ${CMAKE_OBJDUMP}  -x ${target} > ${output}
        COMMENT "Generating map file ${output}" )
endmacro()

macro(gen_start_bin target output)
    add_custom_command(TARGET ${target}
        POST_BUILD
        DEPENDS ${target}
        COMMAND ${CMAKE_OBJCOPY}  --output-format=binary ${target} ${output}
        COMMENT "Generating bin file ${output}" )
endmacro()

macro(gen_symbol_size target output)
    add_custom_command(TARGET ${target}
        POST_BUILD
        DEPENDS ${target}
        COMMAND ${CMAKE_NM} --print-size --size-sort ${target} > ${output}
        COMMENT "Generating symbol size file ${output}" )
endmacro()

macro(gen_iso target bootbin output)
    add_custom_command(TARGET ${target}
        POST_BUILD
        DEPENDS     ${bootbin}
        COMMAND     $(YAUL_INSTALL_ROOT)/bin/make-ip
        ARGS        ${bootbin} ${IP_VERSION} ${IP_RELEASE_DATE} ${IP_AREAS} ${IP_PERIPHERALS} "${IP_TITLE}" ${IP_MASTER_STACK_ADDR} ${IP_SLAVE_STACK_ADDR} ${IP_1ST_READ_ADDR} ${IP_1ST_READ_SIZE} 
        COMMENT     "Generating IP.bin")

    add_custom_command(TARGET ${target}
        POST_BUILD
        DEPENDS     ${CMAKE_BINARY_DIR}/IP.bin
        # COMMAND     $(YAUL_INSTALL_ROOT)/bin/make-iso 
        # ARGS        ${CMAKE_SOURCE_DIR}/cd ${CMAKE_BINARY_DIR}/IP.BIN ${output}

        COMMAND     xorrisofs -quiet -sysid "SEGA SEGASATURN" -volid "FENRIR LOADER" -volset "FENRIR LOADER" -publisher "SEGA ENTERPRISES, LTD." -preparer "SEGA ENTERPRISES, LTD." -appid "SEGA ENTERPRISES, LTD." 
                    -full-iso9660-filenames
                    -generic-boot ${CMAKE_BINARY_DIR}/IP.bin -abstract "ABS.TXT" -biblio "BIB.TXT" -copyright "CPY.TXT" -verbose
                    -o "${output}" ${bootbin}
        COMMENT     "Generating disc image for ${target}")
endmacro()