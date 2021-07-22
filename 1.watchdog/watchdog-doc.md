# 概要
### common
- 看门狗通用API，可直接拷贝自行使用
### watchdog_test1
- 简单看门狗demo，设置看门狗时间20秒，且不喂狗
### watchdog_test2
- 命令形式，测试看门狗，具体命令格式可见运行后的提示。
# 使用说明
### kernel下配置
- 执行 make menuconfig 打开看门狗驱动（Device Drivers  --->[*] SStar SoC platform drivers  ---><*>   watchdog driver）
- 重新编译烧录kernel后，可见板上存在的 /dev/watchdog
### 限制
- 目前最小时间5秒，最大时间357秒（驱动限制，设置超过357秒后会溢出，如设置了400秒，实际设置的是 400-357=43秒）