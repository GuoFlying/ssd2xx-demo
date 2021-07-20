#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "gpio_cfg.h"

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <poll.h>

#include "gpio_cfg.h"

// gpio引脚定义
#define GPIO_LED 10
#define GPIO_KEY 11

// LED状态控制
#define LED_ON() gpio_set_value(GPIO_LED, GPIO_LOW)
#define LED_OFF() gpio_set_value(GPIO_LED, GPIO_HIGH)

int main(int argc, char *argv[])
{
    int fd = -1;
    int ret = -1;
    nfds_t nfds = 1;      // 指定fds数组中的项目数
    struct pollfd fds[1]; // 指定要监视的文件描述符集
    int timeout = 0;      // poll超时时间(单位: 毫秒)
    char gpio_ch = 0;     // 读取到的gpio值
    uint8_t cnt = 0;      // 记录读取次数

    // 导出各gpio
    gpio_export(GPIO_LED);
    gpio_export(GPIO_KEY);

    // 设置gpio方向
    gpio_set_direction(GPIO_LED, GPIO_OUT);
    gpio_set_direction(GPIO_KEY, GPIO_IN);

    // 设置gpio 下降沿触发 --SSD2xx不支持边缘触发
    gpio_set_edge(GPIO_KEY, GPIO_FALLING);

    // 关闭LED和蜂鸣器
    LED_OFF();

    // 打开设备
    fd = gpio_open(GPIO_KEY);
    if (-1 == fd)
    {
        perror("open gpio key fail");

        return -1;
    }

    // 判断按键是否被按下
    while (1)
    {
        // 设置超时时间为1s
        timeout = (1 * 1000);

        // 设置需要监听的文件描述符
        // 监听gpio时, events需要使用POLLPRI(表示有紧急数据需要读取), 不可使用POLLIN
        memset(fds, 0, sizeof(fds));
        fds[0].fd = fd;
        fds[0].events = POLLPRI;

        ret = poll(fds, nfds, timeout);
        // 返回负值, 发生错误
        if (ret < 0)
        {
            perror("poll error");

            return -1;
        }
        // 返回0, 超时
        else if (0 == ret)
        {
            continue;
        }
        // 返回值大于0, 成功
        else
        {
            // 判断是否是期望的返回
            if (fds[0].revents & POLLPRI)
            {
                // 从文件起始位置开始读数据
                lseek(fds[0].fd, 0, SEEK_SET);
                ret = read(fds[0].fd, &gpio_ch, 1);
                if ((1 == ret) && (cnt > 0))
                {
                    usleep(20 * 1000); // 按键消抖

                    enum gpio_value value = GPIO_LOW;
                    gpio_get_value(&value, GPIO_KEY);
                    // 释放按键(上升沿), 熄灭LED, 关蜂鸣器
                    if ((GPIO_HIGH == value) && (0x31 == gpio_ch))
                    {
                        printf("release button\n");

                        // 熄灭LED
                        LED_OFF();
                    }
                    // 按下按键(下降沿), 点亮LED, 开蜂鸣器
                    else
                    {
                        printf("press button\n");

                        // 点亮LED
                        LED_ON();
                    }
                    cnt = 0; // 读取次数清零
                }
                cnt++; // 放在判断后增加, 防止首次读取到就执行
            }
        }
    }

    gpio_close(fd);

    return 0;
}