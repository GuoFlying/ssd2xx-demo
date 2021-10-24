# 公板触摸屏不准设置

- 触摸驱动的配置需要跟板子上的 `/config/fbdev.ini` 的分辨率对应

## 设置方法

- 文件见 `doc/other/tp_config`
- 设置命令如下

  ```shell
    echo 1024x600.bin > /sys/bus/i2c/devices/0-005d/gtcfg
    echo 800x480.bin > /sys/bus/i2c/devices/0-005d/gtcfg
    #or
    echo 1024x600.bin > /sys/bus/i2c/devices/1-005d/gtcfg
    echo 800x480.bin > /sys/bus/i2c/devices/1-005d/gtcfg
  ```
