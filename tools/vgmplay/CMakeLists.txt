cmake_minimum_required(VERSION 3.0)
project(vgmtest)

set(SRCS
    src/vgm.c
    src/qsound.c
    src/play.c
)

    
add_executable(vgmtest ${SRCS})
target_include_directories(vgmtest PUBLIC /workspaces/esp32-saturn/lib/vgm/)
target_link_libraries(vgmtest m)
