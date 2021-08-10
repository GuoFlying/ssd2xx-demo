
project(ssd20x_player_app)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/panel)

file(GLOB SRC_FILES "./*.c" "panel/*.c")
file(GLOB HEAD_FILES "./*.h" "panel/*.h")

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread -ldl  -lmi_common  -lmi_sys -lmi_disp -lmi_panel")

