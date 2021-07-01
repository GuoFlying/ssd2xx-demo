# 概要

## ssd20x-ttl-panel-base
-  v030版本的SDK验证
-  显示屏为SAT070CP50_1024x600。**内涵其他点屏屏参**
-  kernel使用配置为 `make infinity2m_ssc011a_s01a_minigui_defconfig`。
-  可点sigmastar公板默认屏
-  ARGB8888
-  使用
```shell
# 后台运行，之后在运行ui的应用，只初始化panel
    ./ssd20x-ttl-panel-base &  
# 简单fb填充背景色，ARGB8888。 如填充红色
    ./ssd20x-ttl-panel-base 0xffff0000 
```

## ssd20x-st7789-240x240-sw
-  v030版本的SDK验证
-  TTL-RGB565小屏
-  还需要通过SPI初始化，**spi软件模拟**发送9bit数据
-  other文件夹中有一些设备树等其他文件的参考

## ssd20x-st7789-240x240-hw
-  v030版本的SDK验证
-  TTL-RGB565小屏
-  还需要通过SPI初始化，**spi硬件外设**发送9bit数据(kernel记得打开spi配置)
-  other文件夹中有一些设备树等其他文件的参考