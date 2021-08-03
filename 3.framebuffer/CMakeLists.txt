
set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/3.framebuffer)
include_directories(${CMAKE_SOURCE_DIR}/3.framebuffer/common)
add_subdirectory(common)
add_subdirectory(fb_info)
add_subdirectory(fb_rect_test)
add_subdirectory(bmp_img_24bit)
add_subdirectory(jpeg_img)

if(CHIP STREQUAL "ssd20x")

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()

add_custom_target(
    cp-framebuffer ALL
COMMAND 
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/res/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${EXECUTABLE_OUTPUT_PATH}/res/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${EXECUTABLE_OUTPUT_PATH}/res/"
)