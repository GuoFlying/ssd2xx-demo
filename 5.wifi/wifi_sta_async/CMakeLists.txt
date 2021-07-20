project(wifi_sta_async)

file(GLOB SRC_FILES ./*.c)
file(GLOB HEAD_FILES ./*.h )
include_directories(${CMAKE_SOURCE_DIR}/0.common/list)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.c ${SRC_FILES} ${HEAD_FILES})

target_link_libraries(${PROJECT_NAME} "-lpthread -ldl -lmi_common -lmi_sys -lmi_wlan -lcjson")
target_link_libraries(${PROJECT_NAME} common-list)
