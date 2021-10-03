
project(alsa_playback_1)

file(GLOB SRC_FILES   "./*.c")
file(GLOB HEAD_FILES  "./*.h")


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/alsa)

#libalsa
include_directories(${CMAKE_SOURCE_DIR}/0.common/alsa/stm32mp157/include)
link_directories(${CMAKE_SOURCE_DIR}/0.common/alsa/stm32mp157/lib)

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lasound")
