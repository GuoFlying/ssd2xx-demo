cmake_minimum_required(VERSION 3.10)
project(ssd2xx-demo)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

string(ASCII 27 Esc)
set(Red         "${Esc}[31m")
set(Green       "${Esc}[32m")
set(ColourReset "${Esc}[m")

set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(CHIP STREQUAL "ssd20x")
    message("${Green} 当前使用demo适用于：${CHIP} ${ColourReset}")
    add_subdirectory(1.watchdog)
elseif(CHIP STREQUAL "ssd2x2")
    message("${Green} 当前使用demo适用于：${CHIP} ${ColourReset}")
    add_subdirectory(1.watchdog)
else()
    message("${Red} 请指定编译目标。eg：cmake -D CMAKE_TOOLCHAIN_FILE=toolchain.cmake -D CHIP=ssd20x .. ${ColourReset}" )
    message("${Red} 编译详细过程可见 README.md ${ColourReset}" )
    message(FATAL_ERROR)
endif()


# set(PROJECT_PATH "/home/flying20x/workspace/ssd20x/project")

# INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
# INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/iniparser)
# INCLUDE_DIRECTORIES("${PROJECT_PATH}/release/include")
# link_directories("${PROJECT_PATH}/release/nvr/i2m/common/glibc/8.2.1/ex_libs/dynamic")
# link_directories("${PROJECT_PATH}/release/nvr/i2m/common/glibc/8.2.1/mi_libs/dynamic")


# file(GLOB_RECURSE INCLUDES "./*.h" )
# #file(GLOB_RECURSE SOURCES  "iniparser/*.c" )

# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

# add_executable(${PROJECT_NAME} main.c ${SOURCES} ${INCLUDES})
# #target_link_libraries(load_audio_tmp "-lpthread -ldl  -lmi_common  -lmi_sys -lmi_disp -lmi_panel")
# target_link_libraries(${PROJECT_NAME} "-lpthread -ldl")



