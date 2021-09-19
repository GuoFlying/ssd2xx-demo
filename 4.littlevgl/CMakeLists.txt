set(_result TRUE)

macro(exists_file_lvgl)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/lvgl.h)
        set(_result FALSE)
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_demos/lv_demo_conf_template.h)
        set(_result FALSE)
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_drivers/lv_drv_conf_template.h)
        set(_result FALSE)
    endif()
    
endmacro(exists_file_lvgl)


macro(add_static_lib _name _path)

    file(GLOB_RECURSE SRC_FILES   "${_path}/*.c")
    file(GLOB_RECURSE HEAD_FILES  "${_path}/*.h"  "./*.h"  )
    add_library(${_name} STATIC ${SRC_FILES} ${HEAD_FILES})

    #后面所有参数添加连接库
    foreach(_lib_item ${ARGN})
        target_link_libraries(${_name} ${_lib_item})
    endforeach(_lib_item)
    
endmacro(add_static_lib)

exists_file_lvgl()
if(${_result}) 

    set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/4.littlevgl)
    project(lib_lvgl)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    add_static_lib(f_lvgl lvgl )
    add_static_lib(f_lv_drivers lv_drivers )
    add_static_lib(f_lv_demos lv_demos f_lvgl )

    add_subdirectory(f_apps) 

else() 

    message("若需要编译lvgl则需要更新子仓库，详情见README.md")
    
endif() 


