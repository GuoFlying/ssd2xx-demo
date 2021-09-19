set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/11.mi_gfx)

if(CHIP STREQUAL "ssd20x")
    add_subdirectory(fb_gfx_fill_rect)
    add_subdirectory(fb_gfx_draw_line)
    add_subdirectory(gfx_rotate_yuv420)
    
elseif(CHIP STREQUAL "ssd2x2")
    add_subdirectory(fb_gfx_fill_rect)
    add_subdirectory(fb_gfx_draw_line)

else()

endif()

add_custom_target(
    cp-mi_gfx ALL
COMMAND 
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/res/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${EXECUTABLE_OUTPUT_PATH}/res/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${EXECUTABLE_OUTPUT_PATH}/res/"
)