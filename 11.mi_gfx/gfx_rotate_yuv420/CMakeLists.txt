project(gfx_rotate_yuv420)

file(GLOB SRC_FILES "sstardisp/*.c" "./*.c")
file(GLOB HEAD_FILES "sstardisp/*.h" "./*.h" )

include_directories(${PROJECT_SOURCE_DIR}/sstardisp)
include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread -ldl  -lmi_common  -lmi_sys -lmi_disp -lmi_panel -lmi_gfx -lmi_divp")

