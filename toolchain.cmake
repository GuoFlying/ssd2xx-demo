# the name of the target operating system
set(CMAKE_SYSTEM_NAME linux)
# which compilers to use
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
# adjust the default behaviour of the find commands:
# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
