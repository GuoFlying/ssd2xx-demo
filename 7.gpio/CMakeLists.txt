set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/7.gpio)
include_directories(${CMAKE_SOURCE_DIR}/7.gpio/common)
add_subdirectory(common)
add_subdirectory(gpio_test1)
add_subdirectory(gpio_test2)
add_subdirectory(gpio_test_input)

if(CHIP STREQUAL "ssd20x")

    add_subdirectory(gpio_test_fast_rollover)

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()
