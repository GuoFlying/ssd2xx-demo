set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/13.uuid)

if(CHIP STREQUAL "ssd20x")
    add_subdirectory(shell_uuid_to_mac)
    add_subdirectory(base_c_uuid_read)
    add_subdirectory(mi_sys_uuid_read)

elseif(CHIP STREQUAL "ssd2x2")
    add_subdirectory(shell_uuid_to_mac)
    add_subdirectory(base_c_uuid_read)
    add_subdirectory(mi_sys_uuid_read)

else()

endif()
