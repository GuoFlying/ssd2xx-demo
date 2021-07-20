project(gpio-common)

file(GLOB SRC_FILES ./*.c)
file(GLOB HEAD_FILES ./*.h)

#add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${HEAD_FILES})
add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEAD_FILES})

