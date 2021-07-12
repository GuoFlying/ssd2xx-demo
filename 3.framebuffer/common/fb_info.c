/*------------------------------------------------------------------------------
* @文件名  :    main.cpp
* @作者      :    flying
* @邮件      :    weekend@111.com
* @概要      :    无
* @注意点  :    无
* 修改历史*********************************************************************
* 时间               作者                 描述
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "sstarFb.h"

int fb_printf_base_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    struct fb_fix_screeninfo finfo;
    //获取屏幕固定信息
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information/n");
        return -1;
    }
    printf("\r\n***********fb info**************\r\n");
    //其他参数，要么不支持，要么固定，可见SDK文档
    printf("设备驱动名称: %s\r\n", finfo.id);
    printf("显存起始物理地址: %p\r\n", finfo.smem_start);
    printf("显存大小: %u\r\n", finfo.smem_len);
    printf("每行字节数: %u\r\n", finfo.line_length);

    struct fb_var_screeninfo vinfo;
    long screensize = 0;
    //获取屏幕可变信息
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information/n");
        return -1;
    }
    printf("可见屏幕（像素数）宽度、高度: （%u，%u）\r\n", vinfo.xres, vinfo.yres);
    printf("虚拟屏幕宽度、高度: (%u,%u)\r\n", vinfo.xres_virtual, vinfo.yres_virtual);
    printf("在 (x,y) 方向上的偏移像素数:  (%u,%u)\r\n", vinfo.xoffset, vinfo.xoffset);
    printf("每个像素所占的比特数: %u\r\n", vinfo.bits_per_pixel);
    printf("颜色分量中 (alpha,红色,绿色,蓝色) 的位域信息:\r\n{\r\n");
    printf("    分量起始比特位: (%u,%u,%u,%u)\r\n", vinfo.transp.offset, vinfo.red.offset, vinfo.green.offset, vinfo.blue.offset);
    printf("    颜色分量所占比特长度:(%u,%u,%u,%u)\r\n", vinfo.transp.length, vinfo.red.length, vinfo.green.length, vinfo.blue.length);
    printf("}\r\n");

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    printf("计算得：显示需要  %d(byte), 当前框架总共有 %d 个缓存\n", screensize, finfo.smem_len / screensize);
    return 0;
}

int fb_printf_displayer_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    MI_FB_DisplayLayerAttr_t pstLayerInfo;
    //获取图层信息
    if (ioctl(fd, FBIOGET_DISPLAYLAYER_ATTRIBUTES, &pstLayerInfo))
    {
        printf("Error reading FBIOGET_DISPLAYLAYER_ATTRIBUTES information/n");
        return -1;
    }
    printf("\r\n获取图层信息:\r\n{\r\n");
    printf("    图层在屏幕上的原点(横,纵)坐标: (%u,%u)\r\n", pstLayerInfo.u32Xpos, pstLayerInfo.u32YPos);
    printf("    图层在屏幕上的显示(宽度,高度): (%u,%u)\r\n", pstLayerInfo.u32dstWidth, pstLayerInfo.u32dstHeight);
    printf("    显存分辨率的(宽,高): (%u,%u)\r\n", pstLayerInfo.u32DisplayWidth, pstLayerInfo.u32DisplayHeight);
    printf("    屏幕显示分辩率的(宽,高): (%u,%u)\r\n", pstLayerInfo.u32ScreenWidth, pstLayerInfo.u32ScreenHeight);
    printf("    FB 中的数据是否为预乘数据: %u\r\n", pstLayerInfo.bPreMul);
    printf("    设置图层信息时参数修改掩码位: %u\r\n", pstLayerInfo.u32SetAttrMask);
    switch (pstLayerInfo.eFbColorFmt)
    {
    case E_MI_FB_COLOR_FMT_RGB565:
        printf("    当前的像素格式: RGB565 格式\r\n");
        break;
    case E_MI_FB_COLOR_FMT_ARGB4444:
        printf("    当前的像素格式: ARGB444  格式\r\n");
        break;
    case E_MI_FB_COLOR_FMT_ARGB8888:
        printf("    当前的像素格式: ARGB8888 格式\r\n");
        break;
    case E_MI_FB_COLOR_FMT_ARGB1555:
        printf("    当前的像素格式: ARGB1555 格式\r\n");
        break;
    case E_MI_FB_COLOR_FMT_YUV422:
        printf("    当前的像素格式: YUV422 格式\r\n");
        break;
    default:
        printf("    当前的像素格式: 无效像素格式\r\n");
        break;
    }
    switch (pstLayerInfo.eFbOutputColorSpace)
    {
    case E_MI_FB_OUTPUT_RGB:
        printf("    输出颜色格式: RGB ColorSpace\r\n");
        break;
    case E_MI_FB_OUTPUT_YUV:
        printf("    输出颜色格式: YUV ColorSpace\r\n");
        break;
    default:
        printf("    输出颜色格式: ERROR\r\n");
        break;
    }
    //此接口只在Ikayaki系列的芯片下有效
    //SStarFB叠加层输出到display chain的目的模组。通过配置不同的Display chain，可以选择叠加层参与的图像调整过程。
    switch (pstLayerInfo.eFbDestDisplayPlane)
    {
    case E_MI_FB_DST_OP0:
        printf("    display chain的目的模组: OP0\r\n");
        break;
    case E_MI_FB_DST_OP1:
        printf("    display chain的目的模组: OP1\r\n");
        break;
    default:
        printf("    display chain的目的模组: 不支持\r\n");
        break;
    }
    printf("}\r\n");
    return 0;
}
int fb_printf_rectangl_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }
    MI_FB_Rectangle_t rectangl;
    //获取叠加层在屏幕上显示的区域
    if (ioctl(fd, FBIOGET_SCREEN_LOCATION, &rectangl))
    {
        printf("Error reading FBIOGET_SCREEN_LOCATION information/n");
        return -1;
    }
    printf("\r\n获取叠加层在屏幕上显示的区域:\r\n{\r\n");
    printf("    (Xpos,Ypos,Width,Height): (%u,%u,%u,%u)\r\n", rectangl.u16Xpos, rectangl.u16Ypos, rectangl.u16Width, rectangl.u16Height);
    printf("}\r\n");
    return 0;
}
int fb_printf_show_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    MI_BOOL bShow;
    // 获取叠加层显示状态
    if (ioctl(fd, FBIOGET_SHOW, &bShow))
    {
        printf("Error reading FBIOGET_SHOW information/n");
        return -1;
    }
    printf("\r\n获取叠加层显示状态:\r\n{\r\n");
    bShow == TRUE ? printf("  当前叠加层处于显示状态\r\n") : printf("   当前叠加层处于隐藏状态\r\n");
    printf("}\r\n");
    return 0;
}

int fb_printf_global_alpha_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    MI_FB_GlobalAlpha_t pstAlpha;
    //当前叠加层的 Alpha 设置
    if (ioctl(fd, FBIOGET_GLOBAL_ALPHA, &pstAlpha))
    {
        printf("Error reading FBIOGET_GLOBAL_ALPHA information/n");
        return -1;
    }
    printf("\r\n当前叠加层的 Alpha 设置:\r\n{\r\n");
    printf("    Alpha 叠加使能: %d\r\n", pstAlpha.bAlphaEnable);
    printf("    Alpha 通道使能: %d\r\n", pstAlpha.bAlphaChannel);
    //Alpha0 值,范围 0～255，默认为 255。在 RGB1:5:5:5 格式下，当最高位为 0 时，选择该值作为 Alpha 叠加的 Alpha 值。
    printf("    Alpha0 值: %d(0x%02x)\r\n", pstAlpha.u8Alpha0, pstAlpha.u8Alpha0);
    //Alpha1 值,范围 0～255，默认为 255。在 RGB1:5:5:5 格式下，当最高位为 1 时，选择该值作为 Alpha 叠加的 Alpha 值。
    printf("    Alpha1 值: %d(0x%02x)\r\n", pstAlpha.u8Alpha1, pstAlpha.u8Alpha1);
    printf("    全局 Alpha 值: %d(0x%02x)\r\n", pstAlpha.u8GlobalAlpha, pstAlpha.u8GlobalAlpha);
    printf("}\r\n");

    return 0;
}

int fb_printf_color_key_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    MI_FB_ColorKey_t pstColorKey;
    //获取叠加层的 colorkey
    if (ioctl(fd, FBIOGET_COLORKEY, &pstColorKey))
    {
        printf("Error reading FBIOGET_COLORKEY information/n");
        return -1;
    }
    printf("\r\n图层colorkey信息:\r\n{\r\n");
    printf("    Colorkey 是否使能: %d\r\n", pstColorKey.bKeyEnable);
    printf("    (R:G:B):(0x%02x:0x%02x:0x%02x)\r\n", pstColorKey.u8Red, pstColorKey.u8Green, pstColorKey.u8Blue);
    printf("}\r\n");
    return 0;
}

int fb_printf_cursor_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    MI_FB_CursorAttr_t pstCursorAttr;
    //获取鼠标图层信息
    if (ioctl(fd, FBIOGET_CURSOR_ATTRIBUTE, &pstCursorAttr))
    {
        printf("Error reading FBIOGET_CURSOR_ATTRIBUTE information/n");
        return -1;
    }
    printf("\r\n获取鼠标图层信息:\r\n{\r\n");
    printf("    鼠标位置(横、纵)坐标: (%u,%u)\r\n", pstCursorAttr.u32XPos, pstCursorAttr.u32YPos);
    printf("    鼠标热点(横、纵)坐标: (%u,%u)\r\n", pstCursorAttr.u32HotSpotX, pstCursorAttr.u32HotSpotY);
    printf("    鼠标是否可见: %d\r\n", pstCursorAttr.bShown);
    printf("    设置鼠标图层参数修改掩码位: %d\r\n", pstCursorAttr.u16CursorAttrMask);
    printf("\r\n    鼠标图层alpha信息:\r\n {\r\n");
    printf("        Alpha 叠加使能: %d\r\n", pstCursorAttr.stAlpha.bAlphaEnable);
    printf("        Alpha 通道使能: %d\r\n", pstCursorAttr.stAlpha.bAlphaChannel);
    //Alpha0 值,范围 0～255，默认为 255。在 RGB1:5:5:5 格式下，当最高位为 0 时，选择该值作为 Alpha 叠加的 Alpha 值。
    printf("        Alpha0 值: %d(0x%02x)\r\n", pstCursorAttr.stAlpha.u8Alpha0, pstCursorAttr.stAlpha.u8Alpha0);
    //Alpha1 值,范围 0～255， 默认为 255。在 RGB1:5:5:5 格式下，当最高位为 1 时，选择该值作为 Alpha 叠加的 Alpha 值。
    printf("        Alpha1 值: %d(0x%02x)\r\n", pstCursorAttr.stAlpha.u8Alpha1, pstCursorAttr.stAlpha.u8Alpha1);
    printf("        全局 Alpha 值: %d(0x%02x)\r\n", pstCursorAttr.stAlpha.u8GlobalAlpha, pstCursorAttr.stAlpha.u8GlobalAlpha);
    printf("    }\r\n");
    printf("\r\n    鼠标图层colorkey信息:\r\n {\r\n");
    printf("        Colorkey 是否使能: %d\r\n", pstCursorAttr.stColorKey.bKeyEnable);
    printf("        (R:G:B):(0x%02x:0x%02x:0x%02x)\r\n", pstCursorAttr.stColorKey.u8Red, pstCursorAttr.stColorKey.u8Green, pstCursorAttr.stColorKey.u8Blue);
    printf("    }\r\n");
    printf("\r\n    图标数据信息:\r\n {\r\n");
    printf("        鼠标(宽、高、pitch): (%u,%u,%u)\r\n", pstCursorAttr.stCursorImageInfo.u32Width, pstCursorAttr.stCursorImageInfo.u32Height, pstCursorAttr.stCursorImageInfo.u32Pitch);
    printf("    }\r\n");
    printf("}\r\n");
    return 0;
}

int fb_printf_proc_mi_info(int fd)
{
    if (fd < 0)
    {
        printf("%s Error :fb < 0 \r\n", __func__);
        return -1;
    }

    printf("\r\ncat /proc/mi_modules/fb/mi_fb0\r\n");
    system("cat /proc/mi_modules/fb/mi_fb0");
    return 0;
}
