# 概要
## 注意
- 必须先初始化过panel才可使用这些demo
- SSD20x 需要先后台运行初始化panel的应用，后才能运行fbdemo 
- SSD2x2 平台panel初始化在boot阶段做了，如果有bootlogo显示则证明初始化过panel了

## 说明
### common
- 通用fb接口
- fb 信息打印接口
- fb 简单矩形填充接口

### fb_info
- 打印fb 所有信息demo

### fb_rect_test
- 简单fb矩形填充demo
- 可填充 RGB8888/565/1444/1555类型

### bmp_img_24bit
- 显示24bit的图片到ARGB888上
- eg:`./bmp_img_24bit res/saturation_800x480_24bit.bmp`

### jpeg_img
- 可显示jpg图片到fb上
- eg：
```shell
# 指定libjpeg库路径
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
./jpeg_img res/detail.jpg
```