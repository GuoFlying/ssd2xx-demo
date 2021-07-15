# SSD系列应用demo<a name="demoDoc"></a>

-   [概要](#demoDoc1)
-   [目录](#demoDoc2)
-   [使用说明](#demoDoc3)
-   [demo说明](#demoDoc4)

## 简介<a name="demoDoc1"></a>

里面包含，SSD20x（SSD201、SSD202、SSD203）、SSD21X（SSD212、SSD222）等常用测试DEMO，方便大家验证问题。需要其他的DEMO可联系我。（邮件：weekend@111.com）

## 目录<a name="demoDoc2"></a>

```
/ssd2xx-demo
├── CMakeLists.txt         # cmake lists，工程配置，用于cmake编译
├── toolchain.cmake        # cmake交叉编译配置
├── README.md              # 说明文档总入口
├── 0.common               # 公用lib、include、第三方库等
│   ├── ssd20x             # SSD20x SDK相关库（V030版本）
│   ├── ssd2x2             # SSD2x2 SDK相关库（V013版本）
│   ├── iniparser          # 第三方ini解析库，iniparser
│   └── 
├── 1.watchdog             # 看门狗demo
├── 2.panel                # 点屏相关demo
├── 3.framebuffer          # fb 操作demo
├── 4.littlevgl            # littleVGL demo
├── 5.wifi                 # wifi demo
├── 4.audio                # 音频相关、MI_AI/MI_AO demo
```

## 使用说明<a name="demoDoc3"></a>

#### 编译SSD20x的DEMO
-   交叉编译工具、cmake等工具安装。
-   编译过程如下：
```shell
    mkdir build
    cd build
    cmake -D CMAKE_TOOLCHAIN_FILE=toolchain.cmake -D CHIP=ssd20x ..
    make -j8
```
-   若之前`cmake`过其他CHIP需要删除掉之前cmake出来的文件
```shell
    cd build
    rm * -rf
    cmake -D CMAKE_TOOLCHAIN_FILE=toolchain.cmake -D CHIP=ssd2x2 ..
    make -j8
```
## demo说明<a name="demoDoc4"></a>

-   [0.common 通用库、第三方开源库等 文档说明](./0.common/common-doc.md)；
-   [1.watchdog 看门狗 文档说明](./1.watchdog/watchdog-doc.md)；
-   [2.panel 点屏相关 文档说明](./2.panel/panel-doc.md)；
-   [3.framebuffer fb操作相关demo 文档说明](./3.framebuffer/framebuffer-doc.md)；
-   [4.littlevgl LVGL GUI demo 文档说明](./4.littlevgl/littlevgl-doc.md)；
-   [5.wifi  WIFI demo 文档说明](./5.wifi/wifi-doc.md)；
-   [6.audio  AO/AI等接口DEMO 文档说明](./6.audio/audio-doc.md)；