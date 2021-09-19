set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/5.wifi)

if(CHIP STREQUAL "ssd20x")
    add_subdirectory(wifi_demo1)
    add_subdirectory(wifi_sta_async)
elseif(CHIP STREQUAL "ssd2x2")
    add_subdirectory(wifi_demo1)
    add_subdirectory(wifi_sta_async)
else()

endif()
