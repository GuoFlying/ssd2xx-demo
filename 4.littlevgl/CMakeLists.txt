set(_result TRUE)

macro(exists_file_lvgl)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/lvgl.h)
        set(_result FALSE)
        message("lvgl")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_demos/lv_demo_conf_template.h)
        set(_result FALSE)
        message("lv_demos")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_drivers/lv_drv_conf_template.h)
        set(_result FALSE)
        message("lv_drivers")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_lib_png/lv_png.h)
        set(_result FALSE)
        message("lv_lib_png")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_lib_gif/lv_gif.h)
        set(_result FALSE)
        message("lv_lib_gif")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_lib_split_jpg/tjpgdcnf.h)
        set(_result FALSE)
        message("lv_lib_split_jpg")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_fs_if/lv_fs_if.h)
        set(_result FALSE)
        message("lv_fs_if")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_lib_freetype/lv_freetype.h)
        set(_result FALSE)
        message("lv_lib_freetype")
    endif()

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lv_lib_bmp/lv_bmp.h)
        set(_result FALSE)
        message("lv_lib_bmp")
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
    add_static_lib(f_lv_lib_png lv_lib_png )
    add_static_lib(f_lv_lib_gif lv_lib_gif f_lvgl )
    add_static_lib(f_lv_lib_split_jpg lv_lib_split_jpg )
    add_static_lib(f_lv_fs_if lv_fs_if )

    # 编译freetype相关
    include_directories(${CMAKE_SOURCE_DIR}/0.common/freetype2/include/freetype2)

    if(CHIP STREQUAL "ssd20x")
        link_directories(${CMAKE_SOURCE_DIR}/0.common/freetype2/ssd20x/static)
    elseif(CHIP STREQUAL "ssd2x2")
        link_directories(${CMAKE_SOURCE_DIR}/0.common/freetype2/ssd20x/static)
    else()
        #link_directories(${CMAKE_SOURCE_DIR}/0.common/freetype2/stm32mp157/share)
        link_directories(${CMAKE_SOURCE_DIR}/0.common/freetype2/stm32mp157/static)
    endif()

    add_static_lib(f_lv_lib_freetype lv_lib_freetype "-lfreetype")
    add_static_lib(f_lv_lib_bmp lv_lib_bmp )

    add_subdirectory(f_apps) 
    
else() 

    message("若需要编译lvgl则需要更新子仓库，详情见README.md")
    
endif() 

add_custom_target(
    cp-lvgl ALL
COMMAND 
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/res/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${EXECUTABLE_OUTPUT_PATH}/res/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${EXECUTABLE_OUTPUT_PATH}/res/"
)