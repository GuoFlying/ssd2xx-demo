project(common-general)

file(GLOB SRC_FILES ./*.c)
file(GLOB HEAD_FILES ./*.h)

set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -g -O3")

#add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${HEAD_FILES})
add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEAD_FILES})
