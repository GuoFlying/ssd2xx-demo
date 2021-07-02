project(ssd20x-ttl-panel-base)

file(GLOB SRC_FILES "panel/*.c")
file(GLOB HEAD_FILES "panel/*.h" "./*.h" )

include_directories(${PROJECT_SOURCE_DIR}/panel)

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread -ldl  -lmi_common  -lmi_sys -lmi_disp -lmi_panel")

