project(fb_gfx_draw_line)

file(GLOB SRC_FILES ./*.c)
file(GLOB HEAD_FILES ./*.h)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lmi_sys -lmi_gfx")
