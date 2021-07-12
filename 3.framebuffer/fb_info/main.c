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
#include "fb_info.h"

int main(int argc, char **argv)
{
    int fd = 0;
    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0)
    {
        printf("error: can not open framebuffer device/r/n");
        exit(1);
    }

    fb_printf_base_info(fd);
    fb_printf_displayer_info(fd);
    fb_printf_rectangl_info(fd);
    fb_printf_show_info(fd);
    fb_printf_global_alpha_info(fd);
    fb_printf_color_key_info(fd);

    /*SSD20x V030版本平台上打印光标信息会导致kernel挂掉所以只打印SSD2x2平台*/
#ifdef CHIP_SSD2X2 
    fb_printf_cursor_info(fd);
#endif
    fb_printf_proc_mi_info(fd);
    
    return 0;
}
