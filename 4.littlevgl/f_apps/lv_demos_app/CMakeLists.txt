project(lv_demos_app)

include_directories(${PROJECT_SOURCE_DIR})

#file(GLOB_RECURSE SRC_FILES "../../lv_drivers/*.c" "../../lv_demos/*.c"  "../../lvgl/*.c")
#file(GLOB_RECURSE HEAD_FILES "../../lv_drivers/*.h" "../../lv_demos/*.h"  "../../lvgl/*.h"  "../../*.h" "./*.h"  )

add_executable(${PROJECT_NAME} main.c  ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread")
target_link_libraries(${PROJECT_NAME} f_lvgl f_lv_drivers f_lv_demos)