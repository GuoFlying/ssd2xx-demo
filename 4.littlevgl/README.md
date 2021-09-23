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
   - 1：压力测试
   - 2：键盘播放器
   - 3：widgets 

##### 2、 lv_test_all_app
- lvgl 官方组件测试的全部demo
- ./lv_test_all_app 1024 600 3
   - 可执行 shell_lv_test_all_app.sh 脚本快速预览所有demo
   - 表示使用1024*600的屏幕，使用第3个lvgl的demo

##### 3、 lv_lib_png_test_app
- png 解码测试demo
- ./lv_lib_png_test_app 1024 600 ./res/png_decoder_test.png

##### 4、 lv_lib_gif_test_app
- gif 解码测试demo
- ./lv_lib_gif_test_app 1024 600 ./res/example.gif

##### 5、f_game_2048
- 参考100ask 7.11版本2048
- 2048 小游戏
- ./f_game_2048 1024 600 

##### lv_lib_split_jpg_test_app
- 解码3张jpg图片并切换显示，默认调用./res/1.jpg 2.jpg 3.jpg
- ./lv_lib_split_jpg_test_app 1024 600 

##### 7、lv_lib_freetype_test_app
- LVGL默认只有英文字体，若要用中文就会用到freetype
- ./lv_lib_freetype_test_app 1024 600 ./res/GBK_Mobil.TTF