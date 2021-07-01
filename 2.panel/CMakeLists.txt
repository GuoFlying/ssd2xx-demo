if(CHIP STREQUAL "ssd20x")
    add_subdirectory(ssd20x-ttl-panel-base)
    add_subdirectory(ssd20x-st7789-240x240-sw)
    add_subdirectory(ssd20x-st7789-240x240-hw)
elseif(CHIP STREQUAL "ssd2x2")

else()

endif()
