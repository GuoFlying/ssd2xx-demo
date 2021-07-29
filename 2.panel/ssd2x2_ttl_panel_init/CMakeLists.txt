project(ssd2x2_ttl_panel_init)

file(GLOB SRC_FILES "./*.c")
file(GLOB HEAD_FILES "./*.h"  )


add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lmi_panel -lmi_disp")

