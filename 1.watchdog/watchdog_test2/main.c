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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "watchdog.h"
/* Function definitions */
void uexit(char *me)
{
    fprintf(stderr, "\n"
                    "Usage: %s [-s timeout] [-o]\n"
                    "-s, 启动看门狗，并设置看门狗时间范围：5-357秒\n"
                    "-o, 停止看门狗\n"
                    "必须要有看门狗设备：/dev/watchdog \n",
            me);
    exit(1);
}

int main(int argc, char **argv)
{
    int opt;
    char *end;
    unsigned int time;
    int fd = -1;
    while ((opt = getopt(argc, argv, "s:o")) != -1)
    {
        switch (opt)
        {
        case 'o':
            fd = watchdog_open();
            watchdog_stop(fd);
            break;
        case 's':
            time = (off_t)strtoull(optarg, &end, 10);
            if (*end != '\0')
            {
                uexit(argv[0]);
            }
            fd = watchdog_open();
            watchdog_start(fd, time);
            break;
        default:
            uexit(argv[0]);
            break;
        }
    }

    if (fd > 0)
    {
        watchdog_close(fd);
    }
    else
    {
        uexit(argv[0]);
    }
    return 1;
}
