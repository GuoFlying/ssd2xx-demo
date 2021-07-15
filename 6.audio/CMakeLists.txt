set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/6.audio)


if(CHIP STREQUAL "ssd20x")

    add_subdirectory(ai_ao_ini)

elseif(CHIP STREQUAL "ssd2x2")

else()

endif()
