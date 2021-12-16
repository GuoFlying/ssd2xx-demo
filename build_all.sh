#!/bin/sh
###
 # @Author: Flying
 # @Date: 2021-12-16 09:56:08
 # @LastEditors: Flying
 # @LastEditTime: 2021-12-16 10:40:32
 # @Description: 新建文件
### 

make_op=""
cmake_op="-D CMAKE_TOOLCHAIN_FILE=toolchain.cmake -D CHIP=ssd20x"
for param in "$@"
do
    if [ "${param:0:1}" != "-" ]; then
        echo "$param error"
        exit 1
    fi

    if [ "${param:1:1}" == "j" ]; then
        make_op=$param
    fi

    if [ "$param" == "-ssd2x2" ]; then
        cmake_op="-D CMAKE_TOOLCHAIN_FILE=toolchain.cmake -D CHIP=ssd2x2"
    elif [ "$param" == "-mp157" ]; then
        cmake_op="-D CMAKE_TOOLCHAIN_FILE=toolchain_mp157.cmake -D CHIP=mp157"
    fi
    
    if [ "${param:0:2}" == "-h" ]; then
        echo "eg: $0 -j4"
        echo "eg: $0 -j8 -ssd2x2"
        echo "eg: $0 -j8 -mp157"
        exit 0;
    fi
done

echo "make_op = $make_op "
echo "cmake_op = $cmake_op "

rm build -rf 
mkdir build
cd build
cmake $cmake_op ..
make $make_op