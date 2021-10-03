# alsa 概要

- ALSA全称是Advanced Linux Sound Architecture，中文音译是Linux高级声音体系。ALSA 是Linux内核2.6后续版本中支持音频系统的标准接口程序，由ALSA库、内核驱动和相关测  试开发工具组成，更好的管理Linux中音频系统。

## ALSA架构介绍

ALSA是Linux系统中为声卡提供驱动的内核组件。它提供了专门的库函数来简化相应应用程序的编写。相较于OSS的编程接口,ALSA的函数库更加便于使用。

- ALSA系统包括7个子项目
  - 驱动包alsa-driver
  - 开发包alsa-libs
  - 开发包插件alsa-libplugins
  - 设置管理工具包alsa-utils
  - OSS接口兼容模拟层工具alsa-oss
  - 特殊音频固件支持包alsa-finnware
  - 其他声音相关处理小程序包alsa-tools
- ALSA声卡驱动与用户空间体系结构交互如下图所示
  ![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/20211002113017.png)

## 移植ALSA库及工具

- 移植ALSA主要是移植alsa-lib和alsa-utils。
  - alsa-lib：用户空间函数库, 封装驱动提供的抽象接口, 通过文件libasound.so提供API给应用程序使用。
  - alsa-utils：实用工具包,通过调用alsa-lib实现播放音频(aplay)、录音(arecord) 等工具。

  ![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/20211002113248.png)

ALSA Util是纯应用层的软件，相当于ALSA设备的测试程序，ALSA-Lib则是支持应用API的中间层程序，ALSA-Util中的应用程序中会调用到ALSA-Lib中的接口来操作到我们的音频编解码芯片的寄存器，而lib中接口就是依赖于最底层驱动代码，因此移植ALSA程序的顺序就是先后移植Driver,Lib,Util。

### alsa库下载

- 官网：http://www.alsa-project.org 下载alsa-lib和alsa-utils

![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/20211002113512.png)

- 如图下载
  - alsa-lib-1.2.5.1.tar.bz2
  - alsa-utils-1.2.5.1.tar.bz2

### alsa-lib 编译

- 以MP157为例

```shell
tar -xvf alsa-lib-1.2.5.1.tar.bz2
cd alsa-lib-1.2.5.1/
mkdir build
./configure CC=arm-none-linux-gnueabihf-gcc --host=arm-none-linux  --prefix=$PWD/build  
make  -j8
make install 

```

### alsa-utils 编译

- 以MP157为例

```shell
tar -xvf alsa-utils-1.2.5.1.tar.bz2
cd alsa-utils-1.2.5.1/
mkdir build
./configure  CC=arm-none-linux-gnueabihf-gcc --prefix=$PWD/build  --host=arm-none-linux --with-alsa-inc-prefix=/home/flying/workspace/lib/alsa-lib-1.2.5.1/build/include --with-alsa-prefix=/home/flying/workspace/lib/alsa-lib-1.2.5.1/build/lib --disable-alsamixer --disable-xmlto --disable-nls  
make  -j8
make install 

```
