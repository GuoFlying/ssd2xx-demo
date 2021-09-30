add_subdirectory(lv_demos_app)
add_subdirectory(lv_test_all_app)
add_subdirectory(lv_lib_png_test_app)
add_subdirectory(lv_lib_gif_test_app)
add_subdirectory(f_game_2048)
add_subdirectory(lv_lib_split_jpg_test_app)
add_subdirectory(lv_lib_freetype_test_app)
add_subdirectory(f_game_snake)
add_subdirectory(lv_lib_bmp_test_app)
add_subdirectory(f_tool_file_browser)
add_subdirectory(f_game_tetris)


add_custom_target(
    cp-lvgl-f_apps-sh ALL
COMMAND 
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lv_test_all_app/shell_lv_test_all_app.sh ${EXECUTABLE_OUTPUT_PATH}/shell_lv_test_all_app.sh
COMMENT
    "${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lv_test_all_app/shell_lv_test_all_app.sh ${EXECUTABLE_OUTPUT_PATH}/shell_lv_test_all_app.sh"
)