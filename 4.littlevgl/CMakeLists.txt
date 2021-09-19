set(_result TRUE)

macro(exists_file_lvgl)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/lvgl.h)
        set(_result FALSE)
    endif()
    
endmacro(exists_file_lvgl)

exists_file_lvgl()
if(${_result}) 
    message("正常编译")
    set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/4.littlevgl)
    project(lib_lvgl)

    include_directories(${PROJECT_SOURCE_DIR})

    file(GLOB_RECURSE SRC_FILES "lv_drivers/*.c" "lv_demos/*.c"  "lvgl/*.c")
    file(GLOB_RECURSE HEAD_FILES "lv_drivers/*.h" "lv_demos/*.h"  "lvgl/*.h"  "./*.h"  )
    add_library("lib_lvgl" STATIC ${SRC_FILES} ${HEAD_FILES})

    add_subdirectory(f_apps) 
else() 
    message("若需要编译lvgl则需要更新子仓库，详情见README.md")
endif() 


