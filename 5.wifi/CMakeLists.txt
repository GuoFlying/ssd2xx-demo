set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/5.wifi)

add_subdirectory(wifi_demo1)
add_subdirectory(wifi_sta_async)

if(CHIP STREQUAL "ssd20x")

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()
