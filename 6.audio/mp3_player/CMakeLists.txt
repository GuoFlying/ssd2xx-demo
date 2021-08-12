project(mp3_player)

file(GLOB SRC_FILES   "./*.c")
file(GLOB HEAD_FILES  "./*.h")


include_directories(${PROJECT_SOURCE_DIR})

#libmad
include_directories(${CMAKE_SOURCE_DIR}/0.common/libmad/include)
link_directories(${CMAKE_SOURCE_DIR}/0.common/libmad/lib)

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lmad -lpthread -lm -lmi_sys -lmi_ao -ldl -lmi_common")

add_custom_target(
    cp-audio-libmad ALL
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/lib/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/0.common/libmad/lib ${EXECUTABLE_OUTPUT_PATH}/lib/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/0.common/libmad/lib ${EXECUTABLE_OUTPUT_PATH}/lib/"
)