# 概要
- MI_GFX 使用demo



## fb_gfx_fill_rect
- 使用对fb进行填充
- 该DEMO没有点屏初始化动作，若panel没初始化，需要提前初始化
- 直接运行，全屏绘制成紫色，并在左上角点绘红色方框。ARGB888 

## fb_gfx_draw_line
- 使用GFX对fb进行画线
- 该DEMO没有点屏初始化动作，若panel没初始化，需要提前初始化

## gfx_rotate_yuv420
- 精简原厂demo，可在公版上ARGB888,翻转显示YUV420图片
- eg: `./gfx_rotate_yuv420 ./res/YUV420SP_1280_720_NV12_img.yuv 1280 720`