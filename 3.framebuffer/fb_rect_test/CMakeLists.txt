project(fb_rect_test)

file(GLOB SRC_FILES "./*.c")
file(GLOB HEAD_FILES "./*.h" )

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} framebuffer-common)

