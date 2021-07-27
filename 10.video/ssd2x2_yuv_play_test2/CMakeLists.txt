
project(ssd2x2_yuv_play_test2)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB SRC_FILES "./*.c")
file(GLOB HEAD_FILES "./*.h" )

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread -ldl -lmi_common  -lmi_sys  -lmi_divp -lmi_disp")

