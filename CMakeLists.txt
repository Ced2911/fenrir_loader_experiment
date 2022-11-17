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
PROJECT(FenrirYaul)

set(FENRIR_SRC 
   # src/crt0.S 
    assets/army12.c 
	assets/bitmap.c 
	src/main.c 
	src/noise.c 
	src/reset.c 
	src/vdp1.config.c 
	src/vdp2.config.c 
	src/font/font.c 
	src/fenrir/fenrir.c 
	src/fenrir/fenrir.dir_op.c 
	src/fenrir/fenrir.asserts.c 
	src/fenrir/region.c 
	src/screens/gamelist.c 
	src/screens/gamelist_theme.c 
	src/screens/browser.c 
	src/screens/error.c 
	src/screens/screen.c 
	src/theme.c 
	src/sound_driver/pcm.c 
	src/lz4/lz4.c)

add_executable(${PROJECT_NAME} ${FENRIR_SRC})

target_link_libraries(${PROJECT_NAME} ${YAUL_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE src)

gen_map()
gen_start_bin()
gen_symbol_size()
gen_iso()
