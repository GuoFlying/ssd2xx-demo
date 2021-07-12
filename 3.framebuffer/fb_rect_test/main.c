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
#include "fb_rect.h"

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

    fb_rect_init(fd);
    fb_rect_draw_full(0xff00ff00);
    fb_rect_draw(50, 50, 100, 100, 0xff0000ff);

    return 0;
}
