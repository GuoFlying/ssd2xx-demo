# 概要
## 注意
- 必须先初始化过panel才可使用这些demo
- SSD20x 需要先后台运行初始化panel的应用，后才能运行fbdemo 
- SSD2x2 平台panel初始化在boot阶段做了，如果有bootlogo显示则证明初始化过panel了
- 有用到触摸

## 说明
#### demo使用
- ./littlevgl

默认使用800*480 第三个demo

- ./littlevgl 1024 600 3

表示使用1024*600的屏幕，使用第3个lvgl的demo

- demo说明
```
    0：音乐播放器
    1：压力测试
    2：键盘播放器
    3：widgets 
```
#### lv_demos

- V8.0.1
- https://github.com/lvgl/lv_demos.git

#### lv_drivers

- V8.0.1
- https://github.com/lvgl/lv_drivers.git

#### lvgl

- V8.0.1
- https://github.com/lvgl/lvgl.git