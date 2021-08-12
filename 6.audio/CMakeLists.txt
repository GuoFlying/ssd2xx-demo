set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/6.audio)

if(CHIP STREQUAL "ssd20x")

    add_subdirectory(ai_ao_ini)
    add_subdirectory(mp3_player)

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()


add_custom_target(
    cp-audio ALL
COMMAND 
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/res/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${EXECUTABLE_OUTPUT_PATH}/res/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${EXECUTABLE_OUTPUT_PATH}/res/"
)