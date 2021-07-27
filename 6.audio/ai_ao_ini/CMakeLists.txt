
project(ai_ao_ini)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/0.common/iniparser)
include_directories(${PROJECT_SOURCE_DIR}/audio_par)

file(GLOB_RECURSE INCLUDES  "audio_par/*.h" "./*.h" )
file(GLOB_RECURSE SOURCES   "audio_par/*.c")

# link_directories(${EXECUTABLE_OUTPUT_PATH})

add_executable(load_audio_ao_tmp ao_test_main.c ${SOURCES} ${INCLUDES})
target_link_libraries(load_audio_ao_tmp "-lpthread -ldl -lmi_common  -lmi_sys  -lmi_ao -lmi_ai")
target_link_libraries(load_audio_ao_tmp common-iniparser)

add_executable(load_audio_ai_tmp ai_test_main.c ${SOURCES} ${INCLUDES})
target_link_libraries(load_audio_ai_tmp "-lpthread -ldl -lmi_common  -lmi_sys  -lmi_ao -lmi_ai")
target_link_libraries(load_audio_ai_tmp  common-iniparser)

