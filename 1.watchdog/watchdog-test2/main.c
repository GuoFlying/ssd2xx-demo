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
#include <fcntl.h>
#include <linux/watchdog.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#define WATCHDOG_IOCTL_BASE 'W'
#define WDIOC_KEEPALIVE _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WDIOC_SETTIMEOUT _IOWR(WATCHDOG_IOCTL_BASE, 6, int)

static int watchdog_method_func_start(int second)
{
    int fd;
    fd = open("/dev/watchdog", O_WRONLY);
    if (fd == -1)
    {
        printf("fail to open watchdog device\r\n");
    }
    else
    {
        int timeout = second < 1 ? 1 : second;
        ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
        printf("watchdog set timeout: {%d} second \r\n",second);
    }

    return fd;
}

static void watchdog_method_func_stop(int fd)
{
    printf("watchdog_method_func_stop\n");
    int option = WDIOS_DISABLECARD;
    if (fd > 0)
    {
        ioctl(fd, WDIOC_SETOPTIONS, &option);
        close(fd);
    }
    return;
}

static void watchdog_method_func_feed(int fd)
{

    printf("watchdog_method_func_feed\n");

    if (fd)
    {
        ioctl(fd, WDIOC_KEEPALIVE, 0);
    }

    return;
}

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        return 0;
    }

    printf("main init success\n");
    int s = atoi(argv[1]);

    watchdog_method_func_start(s);
    while (1)
    {
        sleep(-1);
    }
    return 1;
}
