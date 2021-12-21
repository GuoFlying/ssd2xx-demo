<!--
 * @Author: Flying
 * @Date: 2021-12-21 06:25:26
 * @LastEditors: Flying
 * @LastEditTime: 2021-12-21 07:19:50
 * @Description: 新建文件
-->
# 交叉编译
## 环境
- ubuntu20.04
- 交叉编译工具 arm-linux-gnueabihf-gcc 8.2.1 20180802 (GNU Toolchain for the A-profile Architecture 8.2-2018-08 (arm-rel-8.23))

## 下载
- git clone https://github.com.cnpmjs.org/Samsung/rlottie.git

### 解压、配置编译
```shell
# 
cd rlottle
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc  -DCMAKE_INSTALL_PREFIX=$PWD/out -DLIB_INSTALL_DIR=$PWD/out/lib -DARCH=arm ..
make -j8
make install 

# 生成的在out 路径下 
```