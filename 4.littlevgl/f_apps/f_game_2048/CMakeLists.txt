project(f_game_2048)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES  "./*.c")
file(GLOB_RECURSE HEAD_FILES "./*.h"  )

add_executable(${PROJECT_NAME} main.c  ${HEAD_FILES} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} f_lvgl f_lv_drivers)
