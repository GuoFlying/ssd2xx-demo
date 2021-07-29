set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/12.mi_disp)

if(CHIP STREQUAL "ssd20x")

elseif(CHIP STREQUAL "ssd2x2")
    add_subdirectory(ssd2x2_disp_enable)
else()

endif()
