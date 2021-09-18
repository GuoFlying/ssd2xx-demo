add_subdirectory(lv_demos_app)
add_subdirectory(lv_test_all_app)

add_custom_target(
    cp-lvgl-f_apps-sh ALL
COMMAND 
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lv_test_all_app/lv_test_all_app_test.sh ${EXECUTABLE_OUTPUT_PATH}/lv_test_all_app_test.sh
COMMENT
    "${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lv_test_all_app/lv_test_all_app_test.sh ${EXECUTABLE_OUTPUT_PATH}/lv_test_all_app_test.sh"
)