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

static struct fb_rect_t_
{
    char *frameBuffer;
    MI_FB_ColorFmt_e fmt;
    int line_length;
    int yoffset;
    int xoffset;
    int xres;
    int yres;
} g_info;

int fb_rect_init(int fd)
{
    memset(&g_info, 0, sizeof(g_info));

    MI_FB_DisplayLayerAttr_t pstLayerInfo;
    //获取图层信息
    if (ioctl(fd, FBIOGET_DISPLAYLAYER_ATTRIBUTES, &pstLayerInfo))
    {
        printf("Error reading FBIOGET_DISPLAYLAYER_ATTRIBUTES information/n");
        return -1;
    }
    g_info.fmt = pstLayerInfo.eFbColorFmt;

    struct fb_fix_screeninfo finfo;
    //获取屏幕固定信息
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information/n");
        return -1;
    }
    g_info.line_length = finfo.line_length;

    struct fb_var_screeninfo vinfo;
    //获取屏幕可变信息
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information/n");
        return -1;
    }
    g_info.xoffset = vinfo.xoffset;
    g_info.yoffset = vinfo.yoffset;
    g_info.xres = vinfo.xres;
    g_info.yres = vinfo.yres;
    /* Map the device to memory */
    g_info.frameBuffer =
        (char *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                     fd, 0);
    if (g_info.frameBuffer == MAP_FAILED)
    {
        perror("Error: Failed to map framebuffer device to memory");
        return -1;
    }

    return 0;
}

static void fb_rect_draw_rgb32(int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 4;
    const int stride = g_info.line_length / bytesPerPixel;

    int *dest = (int *)(g_info.frameBuffer) + (y0 + g_info.yoffset) * stride + (x0 + g_info.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color;
        }
        dest += stride;
    }
}

static void fb_rect_draw_rgb16(int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = g_info.line_length / bytesPerPixel;
    const int red = (color & 0xff0000) >> (16 + 3);
    const int green = (color & 0xff00) >> (8 + 2);
    const int blue = (color & 0xff) >> 3;
    const short color16 = blue | (green << 5) | (red << (5 + 6));

    short *dest = (short *)(g_info.frameBuffer) + (y0 + g_info.yoffset) * stride + (x0 + g_info.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color16;
        }
        dest += stride;
    }
}

static void fb_rect_draw_rgb1555(int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = g_info.line_length / bytesPerPixel;
    const int red = (color & 0xff0000) >> (16 + 3);
    const int green = (color & 0xff00) >> (8 + 3);
    const int blue = (color & 0xff) >> 3;
    const short color15 = blue | (green << 5) | (red << (5 + 5)) | 0x8000;

    short *dest = (short *)(g_info.frameBuffer) + (y0 + g_info.yoffset) * stride + (x0 + g_info.xoffset);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color15;
        }
        dest += stride;
    }
}

static void fb_rect_draw_rgb1444(int x0, int y0, int width, int height, int color)
{
    const int bytesPerPixel = 2;
    const int stride = g_info.line_length / bytesPerPixel;
    const int red = (color & 0xff0000) >> (16 + 4);
    const int green = (color & 0xff00) >> (8 + 4);
    const int blue = (color & 0xff) >> 4;
    const short color16 = blue | (green << 4) | (red << (4 + 4)) | 0xf000;
    short *dest = (short *)(g_info.frameBuffer) + (y0 + g_info.yoffset) * stride + (x0 + g_info.xoffset);

    int x, y;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dest[x] = color16;
        }
        dest += stride;
    }
}

void fb_rect_draw(int x0, int y0, int width, int height, int color)
{
    if (!g_info.frameBuffer)
    {
        printf("Error: not init fb_rect\r\n");
        return;
    }
    switch (g_info.fmt)
    {
    case E_MI_FB_COLOR_FMT_ARGB8888:
    {
        fb_rect_draw_rgb32(x0, y0, width, height, color);
    }
    break;
    case E_MI_FB_COLOR_FMT_RGB565:
    {
        fb_rect_draw_rgb16(x0, y0, width, height, color);
    }
    break;
    case E_MI_FB_COLOR_FMT_ARGB4444:
    {
        fb_rect_draw_rgb1444(x0, y0, width, height, color);
    }
    break;
    case E_MI_FB_COLOR_FMT_ARGB1555:
    {
        fb_rect_draw_rgb1555(x0, y0, width, height, color);
    }
    break;
    default:
        printf("Warning: fb_rect_draw() not implemented for color Fmt %i\n",g_info.fmt);
    }
}

void fb_rect_draw_full(int color)
{
    fb_rect_draw(0, 0, g_info.xres,  g_info.yres, color);
}