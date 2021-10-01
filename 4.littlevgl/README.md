# 概要

## 注意

- 必须先初始化过panel才可使用这些demo
- SSD20x 需要先后台运行初始化panel的应用，后才能运行fbdemo
- SSD2x2 平台panel初始化在boot阶段做了，如果有bootlogo显示则证明初始化过panel了
- 有用到触摸

## 说明

#### demo使用

- 这边 的 demo 默认前2个参数为设置宽高
- 默认 1024*600

##### 1、 lv_demos_app

- ./lv_demos_app 1024 600 3
  - 表示使用1024*600的屏幕，使用第3个lvgl的demo
- demo说明
  - 0：音乐播放器
  ![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-10.png)

  - 1：压力测试
   ![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-11.png)

  - 2：键盘播放器
   ![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-12.png)

  - 3：widgets
   ![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-13.png)

##### 2、 lv_test_all_app

- lvgl 官方组件测试的全部demo
- ./lv_test_all_app 1024 600 3
  - 可执行 shell_lv_test_all_app.sh 脚本快速预览所有demo
  - 表示使用1024*600的屏幕，使用第3个lvgl的demo

##### 3、 lv_lib_png_test_app

- png 解码测试demo
- ./lv_lib_png_test_app 1024 600 ./res/png_decoder_test.png
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-9.png)

##### 4、 lv_lib_gif_test_app

- gif 解码测试demo
- ./lv_lib_gif_test_app 1024 600 ./res/example.gif
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-8.png)

##### 5、f_game_2048

- 参考100ask 7.11版本2048
- 2048 小游戏
- ./f_game_2048 1024 600
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-7.png)

##### 6、 lv_lib_split_jpg_test_app

- 解码3张jpg图片并切换显示，默认调用./res/1.jpg 2.jpg 3.jpg
- ./lv_lib_split_jpg_test_app 1024 600
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-6.png)

##### 7、lv_lib_freetype_test_app

- LVGL默认只有英文字体，若要用中文就会用到freetype
- ./lv_lib_freetype_test_app 1024 600 ./res/GBK_Mobil.TTF
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-5.png)

##### 8、 lv_lib_bmp_test_app

- bmp 解码测试demo
- ./lv_lib_bmp_test_app 1024 600 ./res/example_16bit.bmp
- 需要自己看下panel 是几位色，对应选择bmp
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-4.png)

##### 9、 f_game_snake

- 简易贪吃蛇小游戏
- ./f_game_snake 1024 600
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-3.png)

##### 10、 f_tool_file_browser

- 简易文件浏览器
- 可进入文件夹上下级
- 并可查看png、jpg、gif、bmp、ttf文件
- ./f_tool_file_browser 1024 600
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-2.png)

##### 11、 f_game_tetris

- 简易俄罗斯方块游戏
- 使用lv_canvas 画布部件制作，可用于属性了解lv_canvas用法
- ./f_game_tetris 1024 600
![图示](https://gitee.com/mFlying/flying_picgo_img/raw/master/frame-1.png)
