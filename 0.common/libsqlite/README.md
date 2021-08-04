# 交叉编译
## 环境
- ubuntu20.04
- 交叉编译工具 arm-linux-gnueabihf-gcc 8.2.1 20180802 (GNU Toolchain for the A-profile Architecture 8.2-2018-08 (arm-rel-8.23))

## 下载
- sqlite官网直接下载 https://www.sqlite.org/
- 在ubuntu下可通过wget直接下载`wget https://www.sqlite.org/snapshot/sqlite-snapshot-202107191400.tar.gz`

### 解压、配置编译
```shell
# 
tar -zxvf sqlite-snapshot-202107191400.tar.gz
cd sqlite-snapshot-202107191400/
mkdir build
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build
make -j8
make install 
```