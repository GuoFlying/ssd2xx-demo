project(base_c_uuid_read)

file(GLOB SRC_FILES ./*.c)
file(GLOB HEAD_FILES ./*.h)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} )
