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
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
  
        int fd_fb;
        struct fb_var_screeninfo var;
        int screen_size;
        unsigned char *fbmem;
        unsigned int line_width;
        unsigned int pixel_width;
        char *end;
        unsigned int black;
        black = strtoul(argv[1], &end, 16);
        if (*end != '\0')
        {
            printf("use: %s 0xffff0000\n", argv[0]);
            return -1;
        }

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

        line_width = var.xres * var.bits_per_pixel / 8;
        pixel_width = var.bits_per_pixel / 8;
        screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
        fbmem = (unsigned char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
        if (fbmem == (unsigned char *)-1)
        {
            printf("can't mmap\n");
            return -1;
        }

        int *dest = (int *)(fbmem);
        for (int i = 0; i < screen_size / 4; ++i)
        {
            dest[i] = black;
        }

        munmap(fbmem, screen_size);
        close(fd_fb);



    while (1)
    {
        sleep(-1);
    }
    return 1;
}
