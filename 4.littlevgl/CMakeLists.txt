set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/4.littlevgl)
project(littlevgl)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES "lv_drivers/*.c" "lv_demos/*.c"  "lvgl/*.c")
file(GLOB_RECURSE HEAD_FILES "lv_drivers/*.h" "lv_demos/*.h"  "lvgl/*.h"  "./*.h"  )

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} pthread)
find_package (Threads)