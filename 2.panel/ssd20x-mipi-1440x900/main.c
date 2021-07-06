/*------------------------------------------------------------------------------
** 版权所有:    (c)   2020 深圳市芯智科技
* @文件名  :    main.cpp
* @作者      :    flying
* @邮件      :    guopengfei@smart-core.com.hk
* @概要      :    无
* @注意点  :    无
* 修改历史*********************************************************************
* 时间               作者                 描述
* 2020-7-21        flying             创建该文件
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include "sstardisp.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)

static MI_DISP_PubAttr_t stDispPubAttr;

int main(int argv,char **argc)
{
	int fd;
    // system("echo 9 > /sys/class/gpio/export");
    // system("echo out > /sys/class/gpio/gpio9/direction");
    // system("echo 1 > /sys/class/gpio/gpio9/value");

    // system("echo 85 > /sys/class/gpio/export");
    // system("echo out > /sys/class/gpio/gpio85/direction");
    // system("echo 1 > /sys/class/gpio/gpio85/value");

    // system("echo 4 > /sys/class/gpio/export");
    // system("echo out > /sys/class/gpio/gpio4/direction");
    // system("echo 1 > /sys/class/gpio/gpio4/value");

    // system("echo 86 > /sys/class/gpio/export");
    // system("echo out > /sys/class/gpio/gpio86/direction");
    // system("echo 0 > /sys/class/gpio/gpio86/value");

    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stDispPubAttr.u32BgColor = YUYV_BLACK;
    sstar_disp_init(&stDispPubAttr);
    printf("panel init success\n");
    
    int fd_fb;
    struct fb_var_screeninfo var;	/* Current var */
    int screen_size;
    unsigned char *fbmem;
    unsigned int line_width;
    unsigned int pixel_width;

    fd_fb = open("/dev/fb0", O_RDWR);
	if (fd_fb < 0)
	{
		printf("can't open /dev/fb0\n");
		return -1;
	}
	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("can't get var\n");
		return -1;
	}

	line_width  = var.xres * var.bits_per_pixel / 8;
	pixel_width = var.bits_per_pixel / 8;
	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
	fbmem = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if (fbmem == (unsigned char *)-1)
	{
		printf("can't mmap\n");
		return -1;
	}

    memset(fbmem, 0, screen_size);
    int *dest = (int *) (fbmem);
    for (int i = 0; i < screen_size/4; ++i)
    {
        dest[i] = 0xffff0000;
    }
    sleep(2);
    for (int i = 0; i < screen_size/4; ++i)
    {
        dest[i] = 0xff00ff00;
    }
    sleep(2);
    for (int i = 0; i < screen_size/4; ++i)
    {
        dest[i] = 0xff0000ff;
    }
    sleep(2);
    for (int i = 0; i < screen_size/4; ++i)
    {
        dest[i] = 0xff00ffff;
    }
	munmap(fbmem , screen_size);
	close(fd_fb);

    while(1){
	    sleep(-1);
	}
    return 1;
}
