# 交叉编译
## 环境
- ubuntu20.04

## 下载
- 下载 https://sourceforge.net/projects/freetype/

### 解压、配置编译
```shell
# 
tar -xvf freetype-2.11.0.tar.xz
cd freetype-2.11.0/
mkdir build
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build --with-zlib=no
# ./configure CC=arm-none-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build --with-zlib=no
make -j8
make install 
```