# 概要
- MI_DISP 使用demo
- DISP是一个视频显示单元，主要功能是对前端输出的图像做硬件拼图，并对硬件拼图后的图像进行颜色空间转换，最终通过HDMI/VGA/MIPI/TTL等接口输出到显示器或LCD。


## disp_enable
- ssd212测试
- 测试应用disp enable之后，对显示的影响
- 原本有显示logo，运行后，会黑掉，因为enable后退出会导致disable。从而不能显示


