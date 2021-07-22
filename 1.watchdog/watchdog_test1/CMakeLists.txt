project(watchdog_test1)

file(GLOB SRC_FILES ./*.c)
file(GLOB HEAD_FILES ./*.h)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} watchdog-common)
