set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/4.littlevgl)
project(lib_lvgl)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES "lv_drivers/*.c" "lv_demos/*.c"  "lvgl/*.c")
file(GLOB_RECURSE HEAD_FILES "lv_drivers/*.h" "lv_demos/*.h"  "lvgl/*.h"  "./*.h"  )
add_library("lib_lvgl" STATIC ${SRC_FILES} ${HEAD_FILES})

add_subdirectory(f_apps)