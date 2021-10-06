set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/mini_audio_examples)
project(mini_audio_examples)


macro(add_exe n s)
    add_executable(${n} ${s})
    target_link_libraries(${n} "-lpthread -ldl -lm")
endmacro(add_exe)

file(GLOB SRC_FILES ./*.c)

foreach(FILE_PATH ${SRC_FILES})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${FILE_PATH})
    add_exe(${FILE_NAME} "${FILE_NAME}.c")
endforeach(FILE_PATH)