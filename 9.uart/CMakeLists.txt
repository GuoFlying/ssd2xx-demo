set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/9.uart)
include_directories(${CMAKE_SOURCE_DIR}/9.uart/common)
add_subdirectory(common)
add_subdirectory(uart_test1)
add_subdirectory(uart_test2)
add_subdirectory(uart_test3_poll)
add_subdirectory(uart_test4_signal)

if(CHIP STREQUAL "ssd20x")

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()
