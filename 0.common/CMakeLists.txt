set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/0.common)

#.a/.so文件都在 lib文件夹中
add_subdirectory(iniparser)
add_subdirectory(general)
add_subdirectory(list)
add_subdirectory(tests)