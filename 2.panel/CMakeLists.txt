set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/2.panel)

if(CHIP STREQUAL "ssd20x")
    add_subdirectory(ssd20x-ttl-panel-base)
    add_subdirectory(ssd20x-st7789-240x240-sw)
    add_subdirectory(ssd20x-st7789-240x240-hw)
    add_subdirectory(ssd20x-mipi-1024x600)
elseif(CHIP STREQUAL "ssd2x2")
    add_subdirectory(ssd2x2_ttl_panel_init)

else()
    
endif()
