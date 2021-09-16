# 交叉编译
## 环境
- ubuntu20.04
- 交叉编译工具 arm-linux-gnueabihf-gcc 8.2.1 20180802 (GNU Toolchain for the A-profile Architecture 8.2-2018-08 (arm-rel-8.23))

## 下载
- 官网，找到 Download–Stable releases 的stable（稳定版本，下载哪个版本看个人需求）
- https://libevent.org/
- 当前下载至今最新文档版本（libevent-2.1.12-stable.tar.gz）

### 解压、配置编译
```shell
# 
tar -zxvf libevent-2.1.12-stable.tar.gz
cd libevent-2.1.12-stable
mkdir build
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build --disable-openssl --disable-static
make -j8
make verify #(测试是否编译成功，看日志，如果全部pass表示编译成功)
make install 
```

### 说明
- 编译后的文件就在自己的`$PWD/build`路径了 