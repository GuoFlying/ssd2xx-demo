# 说明
- SSD20x内置RTC。
- 电流0.001mA（见HDK）
- RTC电压正常是2.0V-3.3V
- 硬件电路可参考HDK。

# 配置说明
- 需要在kernel中打开对应的配置RTC才能使用
- 在kernel 文件夹中`make menuconfig`
![配置说明](https://gitee.com/mFlying/flying_picgo_img/raw/master/PicGo-updata-img/RTC_config.png)

# RTC使用参考
- RTC采用标准的LINUX框架，能够使用统一的接口来操作RTC
- 先使用`data`命令查看当前linux时间，和设置时间
- `hwclock`命令可用来查看RTC的时间
- `hwclock -w`命令可将“data”命令设置的时间写入RTC中
- 之后断电重启后可通过hwlock命令读取RTC时间。（原厂文档参考“RTC使用参考”）