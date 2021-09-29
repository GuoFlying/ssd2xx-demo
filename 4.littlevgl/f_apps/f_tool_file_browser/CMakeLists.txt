project(f_tool_file_browser)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/file_rover)

file(GLOB_RECURSE SRC_FILES "file_rover/*.c" "./*.c")
file(GLOB_RECURSE HEAD_FILES "file_rover/*.h" "./*.h"  )

add_executable(${PROJECT_NAME} main.c  ${HEAD_FILES} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} f_lvgl f_lv_drivers f_lv_lib_png  f_lv_lib_gif f_lv_fs_if f_lv_lib_split_jpg f_lv_lib_bmp f_lv_lib_freetype)
