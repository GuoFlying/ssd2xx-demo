# 交叉编译

## 环境

- ubuntu20.04
- 交叉编译工具 arm-linux-gnueabihf-gcc 8.2.1 20180802 (GNU Toolchain for the A-profile Architecture 8.2-2018-08 (arm-rel-8.23))

## 下载

- 下载地址 https://downloads.sourceforge.net/mad/libmad-0.15.1b.tar.gz
- 在ubuntu下可通过wget直接下载`wget https://downloads.sourceforge.net/mad/libmad-0.15.1b.tar.gz`

### 解压、配置编译

```shell
# 
tar -zxvf libmad-0.15.1b.tar.gz
cd libmad-0.15.1b/
mkdir build
# 先执行下面的命令:这条命令是为了适配高版本的gcc，因为高版本的gcc已经将-fforce-mem去除了:
sed -i '/-fforce-mem/d' configure
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build
#./configure CC=arm-none-linux-gnueabihf-gcc --host=arm-none-linux --prefix=$PWD/build
make -j8
make install 
```