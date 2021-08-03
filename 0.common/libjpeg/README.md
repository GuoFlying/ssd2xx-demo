# 交叉编译
## 环境
- ubuntu20.04
- 交叉编译工具 arm-linux-gnueabihf-gcc 8.2.1 20180802 (GNU Toolchain for the A-profile Architecture 8.2-2018-08 (arm-rel-8.23))

## 下载
- http://www.ijg.org/
- 点击 jpegsrc.v9d.tar.gz 下载（可能会更新版本，不一定叫这个名字）
- 在ubuntu下可通过wget直接下载`wget http://www.ijg.org/files/jpegsrc.v9d.tar.gz`

### 解压、配置编译
```shell
# 
tar -zxvf jpegsrc.v9d.tar.gz
cd jpeg-9d
mkdir build
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build
make -j8
make install 
```