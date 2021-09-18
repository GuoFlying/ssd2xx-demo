add_custom_target(
    cp-uuid-sh ALL
COMMAND 
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/uuid_to_mac.sh ${EXECUTABLE_OUTPUT_PATH}/uuid_to_mac.sh
COMMENT
    "${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/uuid_to_mac.sh ${EXECUTABLE_OUTPUT_PATH}/uuid_to_mac.sh"
)