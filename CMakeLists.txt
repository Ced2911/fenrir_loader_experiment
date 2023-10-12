CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

# Configuration per yaul projects
SET(IP_VERSION "V0.001")
SET(IP_RELEASE_DATE "20210301")
SET(IP_AREAS "JTUBKAEL")
SET(IP_PERIPHERALS "JAMKST")
SET(IP_TITLE "FENRIR-LOADER")
SET(IP_MASTER_STACK_ADDR "0x06100000")
SET(IP_SLAVE_STACK_ADDR "0x06002000")
SET(IP_1ST_READ_ADDR "0x06004000")
SET(IP_1ST_READ_SIZE "0")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/yaul.cmake)

# project...
PROJECT("fenrir-yaul")

SET(FENRIR_DEF "")

# not tested
option(VIDEO_480i "Run in 480i mode" OFF)
if (VIDEO_480i)
	SET(FENRIR_DEF ${FENRIR_DEF} FENRIR_480i)
endif()


set(FENRIR_SRC 
   # assets
    assets/army12.c 
	assets/bitmap.c 

	src/bram.c 
	src/main.c 
	src/reset.c 
	src/vdp1.config.c 
	src/vdp2.config.c 
	src/theme.c 
	src/ui.c 
	src/message_box.c 
	src/font/font.c 
	src/dbg.c

	# fenrir
	src/fenrir/fenrir.c 
	src/fenrir/fenrir.dir_op.c 
	src/fenrir/fenrir.asserts.c 
	src/fenrir/region.c 
	src/fenrir/fileio/fileio.c
	
	# screens
	src/screens/screens.c 
	src/screens/error.c 
	src/screens/bram_backup.c 
	src/screens/bram_restore.c
	src/screens/gamelist.c 
	src/screens/gamelist_theme.c 
	src/screens/browser.c 
	src/screens/options.c 
	src/screens/credits.c 
	
	src/sound_driver/pcm.c 
	src/sound_driver/midi.c 
	src/sound_driver/midi/midi-parser.c 
	
	# compression
	src/lz4/lz4.c
	src/fastlz/fastlz.c

	# mssing
	src/stdlib/qsort.c

	
	src/sound_driver/mml_parser.c

	)

add_executable(${PROJECT_NAME} ${FENRIR_SRC})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${FENRIR_DEF})
target_link_libraries(${PROJECT_NAME} ${YAUL_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE src assets)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/libyaul.ld)



set(LOADER_IP_BIN	${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)

yaul_gen_iso(${PROJECT_NAME} -path-list ${CMAKE_CURRENT_SOURCE_DIR}"/iso-path-list")
yaul_gen_dbg(${PROJECT_NAME})

# create boot.bin 
add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD ${PROJECT_NAME}
	COMMAND cp ${CMAKE_BINARY_DIR}/fenrir-yaul.iso ${CMAKE_BINARY_DIR}/fenrir-yaul-big.iso
	COMMAND truncate -s 300M ${CMAKE_BINARY_DIR}/fenrir-yaul-big.iso
)