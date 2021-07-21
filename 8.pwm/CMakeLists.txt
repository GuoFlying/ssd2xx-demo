set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/8.pwm)
include_directories(${CMAKE_SOURCE_DIR}/8.pwm/common)
add_subdirectory(common)
add_subdirectory(pwm_test1)
add_subdirectory(pwm_test2)

if(CHIP STREQUAL "ssd20x")

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()
