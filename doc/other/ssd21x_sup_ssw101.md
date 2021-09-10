# SSD2x2公板使用SSW101B wifi
### 一、背景
- 文档中没有描述SSW101B的wifi如何使用，但是SDK里面是包含着SSW101B的驱动的。
- 版本V014
### 二、使用步骤
- 以nor为例。
- 在project文件夹中操作
##### 1、选择配置
```shell
# 根据环境搭建文档选择无sdio wifi的配置
make dispcam_p3_nor.glibc-9.1.0-squashfs.s01a.64.qfn128.demo_defconfig
# 打开配置，在里面选择使用ssw101b（sigma_wifi -> (X) ssw101b_wifi_usb）
make menuconfig
```
##### 2、添加脚本写入bootcmd命令
- 在make menuconfig后，可看到日志**IMAGE_CONFIG = nor.squashfs.partition.dualenv.dispcam.config**
- 直接在该文件添加即可`vim image/configs/p3/nor.squashfs.partition.dualenv.dispcam.config`
- 修改如下
```sh
wifi23mclkcmd = mw 1f00lcc0 11
wifirstoffcmd = gpio out 60 1
wifirstoncmd = gpio out 60 0

bootlogocmd = $(wifi23mclkcmd)\;$(wifirstoffcmd)\; bootlogo 0 0 0 0\;$(wifirstoncmd)\;

```
- 目的只是为了把wifi23mclkcmd 、wifirstoffcmd 、wifirstoncmd 写进env中bootcmd的参数里面。也可写在其他地方，可自行调整
- gpio 60对应的是公板的wifi电源引脚，若自己画板，需要自行调整

##### 3、编译
- 直接make image 之后，烧录即可正常使用
- 测试方式可参考**SSD20x WIFI使用测试**