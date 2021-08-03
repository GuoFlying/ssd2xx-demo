project(jpeg_img)

file(GLOB SRC_FILES  "jpeg/*.c" "./*.c")
file(GLOB HEAD_FILES  "jpeg/*.h" "./*.h")


include_directories(${PROJECT_SOURCE_DIR}/jpeg)
include_directories(${PROJECT_SOURCE_DIR})

#指定libjpeg路径
include_directories(${CMAKE_SOURCE_DIR}/0.common/libjpeg/include)
link_directories(${CMAKE_SOURCE_DIR}/0.common/libjpeg/lib)

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-ljpeg")

add_custom_target(
    cp-framebuffer-libjpeg ALL
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/lib/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/0.common/libjpeg/lib ${EXECUTABLE_OUTPUT_PATH}/lib/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/0.common/libjpeg/lib ${EXECUTABLE_OUTPUT_PATH}/lib/"
)