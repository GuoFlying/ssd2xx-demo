
set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/3.framebuffer)
include_directories(${CMAKE_SOURCE_DIR}/3.framebuffer/common)
add_subdirectory(common)
add_subdirectory(fb_info)

if(CHIP STREQUAL "ssd20x")

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()

