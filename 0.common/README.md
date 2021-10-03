# 概要

## ssd2x2

- v014版本的SDK，下的so、include
- 若后续有更新版本可自行复制过来,地址如下:

```shell
    ${PROJECT_PATH}/release/include
    ${PROJECT_PATH}/release/dispcam/p3/common/glibc/9.1.0/ex_libs
    ${PROJECT_PATH}/release/dispcam/p3/common/glibc/9.1.0/mi_libs
```

## ssd20x

- v030版本的SDK，下的so、include
- 若后续有更新版本可自行复制过来,地址如下:

```shell
    ${PROJECT_PATH}/release/include
    ${PROJECT_PATH}/release/nvr/i2m/common/glibc/8.2.1/ex_libs
    ${PROJECT_PATH}/release/nvr/i2m/common/glibc/8.2.1/mi_libs
```

## 通用库

## libfastcommon

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

## libevnet

- Libevent 是一个用C语言编写的、轻量级的开源高性能事件通知库，主要有以下几个亮点：事件驱动（ event-driven），高性能;轻量级，专注于网络，不如 ACE 那么臃肿庞大；源代码相当精炼、易读；跨平台，支持 Windows、 Linux、 *BSD 和 Mac Os；支持多种 I/O 多路复用技术， epoll、 poll、 dev/poll、 select 和 kqueue 等；支持 I/O，定时器和信号等事件；注册事件优先级。
- Libevent 已经被广泛的应用，作为底层的网络库；比如 memcached、 Vomit、 Nylon、 Netchat等等
- 动态链接库、头文件
- [交叉编译说明](./libevent/README.md)

## freetype2

- 是一个免费、开源、可移植且高质量的字体引擎；支持多种字体格式文件，并提供了统一的访问接口；支持单色位图、反走样位图渲染，这使字体显示质量达到Mac的水平；采用面向对象思想设计，用户可以灵活的根据需要裁剪。
- 动态链接库、头文件
- [交叉编译说明](./freetype2/README.md)

## alsa 概要

- ALSA全称是Advanced Linux Sound Architecture，中文音译是Linux高级声音体系。ALSA 是Linux内核2.6后续版本中支持音频系统的标准接口程序，由ALSA库、内核驱动和相关测  试开发工具组成，更好的管理Linux中音频系统。
- 动态链接库、头文件
- [交叉编译说明](./alsa/README.md)