set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/10.video)


if(CHIP STREQUAL "ssd20x")
    add_subdirectory(ssd20x_h264_play_test1)
    add_subdirectory(ssd20x_h264_play_test2)
    add_subdirectory(ssd20x_player_app)
    
elseif(CHIP STREQUAL "ssd2x2")

    add_subdirectory(ssd2x2_yuv_play_test1)
    #add_subdirectory(ssd2x2_yuv_play_test2)

else()
    
endif()


add_custom_target(
    cp-video ALL
COMMAND 
    ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/res/
COMMAND 
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${EXECUTABLE_OUTPUT_PATH}/res/
COMMENT
    "${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${EXECUTABLE_OUTPUT_PATH}/res/"
)