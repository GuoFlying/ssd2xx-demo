project(f_game_tetris)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES   "tetris_logic/*.c" "./*.c" )
file(GLOB_RECURSE HEAD_FILES  "tetris_logic/*.h" "./*.h"  )

add_executable(${PROJECT_NAME} main.c  ${HEAD_FILES} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} "-lm")
target_link_libraries(${PROJECT_NAME} f_lvgl f_lv_drivers)
