project(st7789_240x240_sw)

file(GLOB SRC_FILES "./*.c")
file(GLOB HEAD_FILES "./*.h" )

include_directories(${PROJECT_SOURCE_DIR}/)

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread -ldl  -lmi_common  -lmi_sys -lmi_disp -lmi_panel")

