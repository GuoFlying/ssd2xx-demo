project(common_test)

link_directories(${LIBRARY_OUTPUT_PATH})
include_directories(${CMAKE_SOURCE_DIR}/0.common/general)
include_directories(${CMAKE_SOURCE_DIR}/0.common/iniparser)
include_directories(${CMAKE_SOURCE_DIR}/0.common/list)

macro(add_exe n s)
    add_executable(${n} ${s})
    target_link_libraries(${n} "-lpthread -lm -ldl")
    target_link_libraries(${n} common-general)
    target_link_libraries(${n} common-iniparser)
    target_link_libraries(${n} common-list)
endmacro(add_exe)

file(GLOB SRC_FILES ./*.c)

foreach(FILE_PATH ${SRC_FILES})
    #message(${FILE_PATH})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${FILE_PATH})
    #message("a == ${FILE_NAME}")
    add_exe(${FILE_NAME} "${FILE_NAME}.c")
endforeach(FILE_PATH)

# add_custom_target(test)
# add_custom_command(TARGET test POST_BUILD 
#  COMMAND 
#     ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/0.common/tests/servers.conf ${EXECUTABLE_OUTPUT_PATH}
# COMMENT
#     "cp sonf ini... to bin"
# )

add_custom_target(
    cp-test ALL
COMMAND 
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/servers.conf ${EXECUTABLE_OUTPUT_PATH}
COMMAND 
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.ini ${EXECUTABLE_OUTPUT_PATH}
COMMENT
    "${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/xxx.xx ${EXECUTABLE_OUTPUT_PATH}"
)