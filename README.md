# SSD系列应用demo<a name="demoDoc"></a>

-   [概要](#demoDoc1)
-   [目录](#demoDoc2)
-   [使用说明](#demoDoc3)
-   [内部DEMO说明](#demoDoc4)
-   [SSD2XX的其他外部DEMO](#demoDoc5)
-   [联系我们](#demoDoc6)

## 简介<a name="demoDoc1"></a>

- 里面包含，SSD20x（SSD201、SSD202、SSD203）、SSD21X（SSD212、SSD222）等常用测试DEMO，方便大家验证问题。
- 文件夹名称数字没有特殊含义，只是按添加DEMO前后顺序命名而已

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
│   ├── general            # 第三方通用库，如列表、队列、等
│   ├── tests              # common测试代码
│   ├── list               # 双向链表
│   └── ###                # END
├── 1.watchdog             # 看门狗demo
├── 2.panel                # 点屏相关demo
├── 3.framebuffer          # fb 操作demo
├── 4.littlevgl            # littleVGL demo
├── 5.wifi                 # wifi demo
├── 6.audio                # 音频相关、MI_AI/MI_AO demo
├── 7.gpio                 # GPIO 操作 demo
├── 8.pwm                  # PWM 操作 demo
├── 9.uart                 # UART(串口) 操作 demo
├── 10.video               # 视频、YUV显示等相关demo
└── ###                    # END
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

#### 编译SSD2x2的DEMO
-   若之前`cmake`过其他CHIP需要删除掉之前cmake出来的文件
```shell
    cd build
    rm * -rf
    cmake -D CMAKE_TOOLCHAIN_FILE=toolchain.cmake -D CHIP=ssd2x2 ..
    # 全部编译
    make -j8
    # 可查看有什么demo，之后可指定DEMO编译
    make help 
    make pwm_test1
```
- **编译后生成的文件在/build/bin & /build/lib**

## DEMO说明<a name="demoDoc4"></a>

-   [0.common 通用库、第三方开源库等 文档说明](./0.common/common-doc.md)；
-   [1.watchdog 看门狗 文档说明](./1.watchdog/watchdog-doc.md)；
-   [2.panel 点屏相关 文档说明](./2.panel/panel-doc.md)；
-   [3.framebuffer fb操作相关demo 文档说明](./3.framebuffer/framebuffer-doc.md)；
-   [4.littlevgl LVGL GUI demo 文档说明](./4.littlevgl/littlevgl-doc.md)；
-   [5.wifi  WIFI demo 文档说明](./5.wifi/wifi-doc.md)；
-   [6.audio  AO/AI等接口DEMO 文档说明](./6.audio/audio-doc.md)；
-   [7.gpio  GPIO DEMO 文档说明](./7.gpio/gpio-doc.md)；
-   [8.pwm  pwm DEMO 文档说明](./8.pwm/pwm-doc.md)；
-   [9.uart 串口 DEMO 文档说明](./9.uart/uart-doc.md)；
-   [10.video 视频相关 DEMO 文档说明](./10.video/video-doc.md)；



## SSD2XX的其他DEMO<a name="demoDoc5"></a>
### 一、SSD20x
#### 1、V030版本的SDK中`sdk`文件夹中会带有一些DEMO。如下
```shell
└── verify
    ├── application
    │   ├── bootanimation
    │   ├── jpeg2disp
    │   ├── logo
    ├── feature
    │   ├── disp
    │   ├── fb
    │   │   ├── fb_tc001
    │   │   ├── fb_tc002
    │   │   ├── fb_tc003
    │   │   ├── fb_tc004
    │   │   ├── fb_tc005
    │   │   ├── fb_tc006
    │   │   └── fb_tc007
    │   └── gfx
    │       ├── common
    │       ├── tc_bitblit
    │       ├── tc_blend
    │       ├── tc_fillrect
    │       └── tc_rotate
    └── mi_demo
        ├── 3rdparty
        │   ├── jpeg
        │   ├── libnl
        │   ├── openssl
        │   ├── png
        │   ├── wpa_supplicant
        │   └── z
        ├── amigos
        │   ├── hdmi_convertor
        │   ├── internal
        │   └── preview
        ├── common
        │   ├── dla_fw
        │   ├── iniparser
        │   ├── live555
        │   ├── lwfs
        │   ├── onvif
        │   ├── opencv
        │   ├── res
        │   └── tem
        ├── geonosis
        │   ├── audio_all_test_case
        │   ├── cardv_test
        │   ├── codec
        │   ├── fb
        │   ├── hdmi_test
        │   ├── internal
        │   ├── lib
        │   ├── obj
        │   ├── vdec
        │   └── venc
        └── out
            └── demo

```
#### 2、单独在SDK外的DEMO可找FAE索取
- 不随SDK发布更新 
- 如下只有在V08版本才有，有的demo在新版本已经不能正常编译，需要自己修改
```shell
# Demo_Release.tar.gz
├─Airplay
├─browser
├─BurnKey
├─CloudPlay+Wifi
├─directfb
├─DisplayDemo
├─enable_cipher
├─eventtest 
├─fb&gfx
│  └─feature
│      ├─disp
│      ├─fb
│      │  ├─fb_tc001
│      │  ├─fb_tc002
│      │  ├─fb_tc003
│      │  ├─fb_tc004
│      │  └─fb_tc005
│      └─gfx
├─H264Player
│  └─h264Player
├─IR
├─JpegPlayer
├─jpg_rotate
├─mini_player
├─MM
├─QT
├─RequestIDR
├─rw_reg
├─simplayer
├─smarttalk_outside
├─UI_DEMO
│  ├─directfb
│  ├─eventtest
│  ├─GFX
│  ├─GFX旋转
│  ├─jpg_rotate
│  ├─pri_screensaver
│  ├─pri_statusbar
│  ├─QT
│  └─security_SSDUI
├─usb_camera
├─vdec_getoutpubuf
├─wifi
└─四分屏+PIP
```

### 二、SSD2x2
#### 1、V014版本的SDK中`sdk`文件夹中会带有一些DEMO。如下
```shell
# V014版本 sdk文件夹
├── driver
│   └── SensorDriver
└── verify
    ├── application
    │   ├── disp_pic_fastboot
    │   ├── jpeg2disp
    │   ├── logo
    │   ├── ota_test
    │   ├── qfn68_sensor_panel
    │   ├── smarthome
    │   ├── smarttalk
    │   ├── ssplayer
    │   ├── usbcamera
    │   └── usbcamera_fastboot
    ├── feature
    │   ├── fb
    │   │   ├── common
    │   │   ├── fb_clrkey
    │   │   ├── fb_color
    │   │   ├── fb_cursor
    │   │   ├── fb_dbbuf
    │   │   ├── fb_dispattr
    │   │   ├── fb_multi
    │   │   ├── fb_scale
    │   │   └── fb_total
    │   └── gfx
    │       ├── common
    │       ├── tc_bitblit
    │       ├── tc_blend
    │       ├── tc_fillrect
    │       └── tc_rotate
    └── mi_demo
        ├── 3rdparty
        ├── common
        │   ├── dla_fw
        │   ├── iniparser
        │   ├── lwfs
        │   ├── onvif
        │   ├── opencv
        │   ├── res
        │   ├── tem
        │   ├── uac
        │   └── uvc
        └── ikayaki
            ├── audio
            ├── audio_alg_AEC_demo
            ├── audio_alg_AED_demo
            ├── audio_alg_APC_AGC_demo
            ├── audio_alg_APC_ANR_demo
            ├── audio_alg_APC_demo
            ├── audio_alg_APC_EQ_demo
            ├── audio_alg_BF_demo
            ├── audio_alg_SRC_demo
            ├── audio_alg_SSL_demo
            ├── audio_all_alg_test_case
            ├── audio_all_test_case
            ├── audio_g711_codec_demo
            ├── audio_g726_codec_demo
            ├── internal
            ├── mix
            ├── panel
            ├── uvc
            ├── venc
            └── vpe

```

#### 2、单独在SDK外的DEMO可找FAE索取
- 不随SDK发布更新
```shell
# IKAYAKI_ReleaseDemo_20210528_f8248bc.tar.gz
├─audio
├─fb_demo
├─fb_gfx
├─JpegPlayer
├─panel
│  ├─PspiDemo
│  ├─SpiPanel
│  └─YuvToPanel
├─rotate
│  └─rotate420
│      ├─sstardisp
│      └─yuv-picture
├─scanpen
│  └─internal
│      ├─common
│      ├─rgn
│      ├─vif
│      └─vpe
├─sensor
│  └─internal
│      ├─common
│      ├─ive
│      ├─rgn
│      ├─spi
│      ├─vif
│      └─vpe
├─uvc
└─wifi
```

### 三、其他
#### 1、原厂给的其他DEMO
- [github路径 https://github.com/aaron201912?tab=repositories](https://github.com/aaron201912?tab=repositories)
- 内涵如工厂自带的中科flthings DEMO等，如下所示
```shell
├─ SSD26X_ZK
├─ ffmpeg               #ffmpeg 交叉编译，使用demo
├─ SSC268
├─ SSD222_Demo 
├─ UuidSSDPlayer        #视频播放器
├─ hp_customer
├─ RunTimeDemo          #SSD20x与SSD2x2 的ZK UI DEMO。新版本
├─ amigos_customer
├─ Qt5.15_example       # QT DEMO，内涵交叉编译说明
├─ rtsp-client
├─ 3rdparty
├─ SSD20xDemo           
├─ SSD_PLAYER           #基于ZK UI播放器的IDE。旧版本
├─ DisplayDemo          #广告发布机Demo
└─ VideoStream
```

## 联系我们<a name="demoDoc6"></a>
- 有问题可直接提 `issues`
- 邮件： `weekend@111.com`