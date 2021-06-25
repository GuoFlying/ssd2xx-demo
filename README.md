# SSD系列应用demo<a name="demoDoc"></a>

-   [概要](#demoDoc1)
-   [目录](#demoDoc2)
-   [约束](#section119744591305)
-   [使用说明](#section741617511812)
    -   [准备](#section1579912573329)
    -   [获取源码](#section11443189655)
    -   [编译构建](#section2081013992812)

-   [相关仓](#section1371113476307)

## 简介<a name="demoDoc1"></a>

里面包含，SSD20x（SSD201、SSD202、SSD203）、SSD21X（SSD212、SSD222）等常用测试DEMO，方便大家验证问题。需要其他的DEMO可联系我。（邮件：weekend@111.com）

## 目录<a name="demoDoc2"></a>

```
/ssd2xx-demo
├── doc                    # makedown文档、图片等
├── CMakeLists.txt         # cmake lists，工程配置，用于cmake编译
├── toolchain.cmake        # cmake交叉编译配置
├── README.md              # 说明文档总入口
├── 0.common               # 公用lib、include、第三方库等
│   ├── ssd20x             # SSD20x SDK相关库（V030版本）
│   │   ├── ex_libs        # 扩展lib
│   │   ├── mi_lisb        # mi lib
│   │   └── include        # ex、mi对应头文
│   └── ssd2x2             # SSD2x2 SDK相关库（V013版本）
│   │   ├── ex_libs        # 扩展lib
│   │   ├── mi_lisb        # mi lib
│   │   └── include        # ex、mi对应头文
└── 1.watchdog             # 看门狗demo

```

## 约束<a name="ssd-demo-3"></a>

-   开发语言：C/C++；
-   适用于Hi3518EV300、Hi3516DV300单板；
-   Hi3518EV300默认使用jffs2文件系统，Hi3516DV300默认使用FAT文件系统。

## 使用说明<a name="section741617511812"></a>

#### 编译SSD20x的DEMO
-   cmake -D CMAKE_TOOLCHAIN_FILE=ssd21x.cmake ..

### 准备<a name="section1579912573329"></a>

开发者需要在Linux上搭建编译环境：

-   Hi3518EV300单板：参考[环境搭建](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/Hi3518%E5%BC%80%E5%8F%91%E6%9D%BF.md)；
-   Hi3516DV300单板：参考[环境搭建](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/Hi3516%E5%BC%80%E5%8F%91%E6%9D%BF.md)。

### 获取源码<a name="section11443189655"></a>

在Linux服务器上下载并解压一套源代码，获取源码（[下载链接](https://repo.huaweicloud.com/harmonyos/os/1.0.1/code-1.0.1.tar.gz)）。更多源码获取方式，参考[源码获取](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/%E6%BA%90%E7%A0%81%E8%8E%B7%E5%8F%96.md)。

### 编译构建<a name="section2081013992812"></a>

开发者开发第一个应用程序可参考：

-   [helloworld for Hi3518EV300](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/%E8%BF%90%E8%A1%8CHello-OHOS-4.md)；

-   [helloworld for Hi3516DV300](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/%E8%BF%90%E8%A1%8CHello-OHOS.md)。

## 相关仓<a name="section1371113476307"></a>

[内核子系统](https://gitee.com/openharmony/docs/blob/master/zh-cn/readme/%E5%86%85%E6%A0%B8%E5%AD%90%E7%B3%BB%E7%BB%9F.md)

[drivers\_liteos](https://gitee.com/openharmony/drivers_liteos/blob/master/README_zh.md)

**kernel\_liteos\_a**

