# 概要
## ssd2x2
-  v014版本的SDK，下的so、include
-  若后续有更新版本可自行复制过来,地址如下:
```shell
    ${PROJECT_PATH}/release/include
    ${PROJECT_PATH}/release/dispcam/p3/common/glibc/9.1.0/ex_libs
    ${PROJECT_PATH}/release/dispcam/p3/common/glibc/9.1.0/mi_libs
```

## ssd20x
-  v030版本的SDK，下的so、include
-  若后续有更新版本可自行复制过来,地址如下:
```shell
    ${PROJECT_PATH}/release/include
    ${PROJECT_PATH}/release/nvr/i2m/common/glibc/8.2.1/ex_libs
    ${PROJECT_PATH}/release/nvr/i2m/common/glibc/8.2.1/mi_libs
```
## 通用库
#### libfastcommon
- 从开源项目FastDFS中提取的C公共函数库，这个库非常简单和稳定。提供的函数功能包括：字符串、日志、链表、哈希表、网络通信、ini配置文件读取、base64编码/解码、url编码/解码、时间轮计时器（timer）、跳表（skiplist）、对象池和内存池等等。详细信息请参阅C头文件。
- 源码路径：https://gitee.com/fastdfs100/libfastcommon.git
- 当前版本 tag: V1.0.53

## iniparser
- ini 解析库
- 源码路径：https://gitee.com/mirrors/iniparser.git
- 当前版本 commit deb85ad4936d4ca32cc2260ce43323d47936410d 

## list
- 双链表
- 源码路径：https://github.com/clibs/list.git
- 当前版本 commit 6c16b69f14de870ed89d0bcac948d85b7a73513f 
- [详细说明](./list/Readme.md)
- [历史说明](./list/History.md)

## test
- 0.common 测试相关代码
- 详细说明见代码

## libjpeg
- libjpeg是一个完全用C语言编写的库，包含了被广泛使用的JPEG解码、JPEG编码和其他的JPEG功能的实现。
- 动态链接库、头文件
- [交叉编译说明](./libjpeg/README.md)

## libsqlite
- SQLite，是一款轻型的数据库，是遵守ACID的关系型数据库管理系统，它包含在一个相对小的C库中。它是D.RichardHipp建立的公有领域项目。
- 动态链接库、头文件
- [交叉编译说明](./libsqlite/README.md)

## libmad
- MAD （libmad）是一个开源的高精度 MPEG 音频解码库，支持 MPEG-1（Layer I, Layer II 和 LayerIII（也就是 MP3）。LIBMAD 提供 24-bit 的 PCM 输出，完全是定点计算，非常适合没有浮点支持的平台上使用。使用 libmad 提供的一系列 API，就可以非常简单地实现 MP3 数据解码工作。在 libmad 的源代码文件目录下的 mad.h 文件中，可以看到绝大部分该库的数据结构和 API 等。
- 动态链接库、头文件
- [交叉编译说明](./libmad/README.md)