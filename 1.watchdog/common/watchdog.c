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
#include "watchdog.h"

//SSD2xx平台目前最大的看门狗时间为357秒。最小的看门狗时间为5秒
#define MAX_TIME_OUT 357
#define MIN_TIME_OUT 5
#define WATCHDOG_IOCTL_BASE 'W'
#define WDIOC_KEEPALIVE _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WDIOC_SETTIMEOUT _IOWR(WATCHDOG_IOCTL_BASE, 6, int)

int watchdog_open()
{
    int fd;
    fd = open(WATCHDOG_DEV_NAME, O_WRONLY);
    if (fd == -1)
    {
        printf("fail to open watchdog device\r\n");
    }
    return fd;
}

void watchdog_start(int fd, int second)
{
    int timeout = second;
    if (second < MIN_TIME_OUT)
    {
        timeout = MIN_TIME_OUT;
        printf("watchdog set timeout < MIN_TIME_OUT \r\n");
    }
    else if (second > MAX_TIME_OUT)
    {
        timeout = MAX_TIME_OUT;
        printf("watchdog set timeout > MAX_TIME_OUT \r\n");
    }

    if (fd > 0)
    {
        ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
        printf("watchdog set timeout: {%d} second \r\n", second);
    }
}

void watchdog_stop(int fd)
{
    int option = WDIOS_DISABLECARD;
    if (fd > 0)
    {
        printf("%s\n", __func__);
        ioctl(fd, WDIOC_SETOPTIONS, &option);
        close(fd);
    }
}

void watchdog_feed(int fd)
{
    if (fd)
    {
        printf("%s\n", __func__);
        ioctl(fd, WDIOC_KEEPALIVE, 0);
    }
}

void watchdog_close(int fd)
{
    if (fd)
    {
        printf("%s\n", __func__);
        close(fd);
    }
}