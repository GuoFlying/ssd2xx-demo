#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <string.h>
#include <pthread.h>
#include "uart_cfg.h"

#define UART_PATH "/dev/ttyS1"

static void *uart_recv_pthread(void *pdata)
{
    int fd = (int)pdata;
    int ret = -1;
    nfds_t nfds = 1;         // 指定fds数组中的项目数
    struct pollfd fds[1];    // 指定要监视的文件描述符集
    int total_data_len = 0;  // 已读取数据长度
    int remain_data_len = 0; // 未读取数据长度
    uint8_t recv_data[1024] = {0};
    uint32_t recv_data_len = 1023;

    while (1)
    {
        // 设置需要监听的文件描述符
        memset(fds, 0, sizeof(fds));
        fds[0].fd = fd;
        fds[0].events = POLLIN;

        ret = poll(fds, nfds, 500); //500ms超时
        if (ret < 0)
        {
            perror("Error：poll");
            return NULL;
        }
        // 返回0, 超时
        else if (0 == ret)
        {
            // 如果超时后, 已读取数据长度大于0, 返回实际接收数据长度
            if (total_data_len > 0)
            {
                printf("recv ： %s\r\n", recv_data);
                memset(recv_data, 0, sizeof(recv_data));
                total_data_len = 0;
                remain_data_len = 0;
            }

            continue;
        }

        // 判断是否是期望的返回
        if (fds[0].revents & POLLIN)
        {
            // 从文件起始位置开始读数据
            lseek(fds[0].fd, 0, SEEK_SET);
            ret = uart_read_data(fds[0].fd, &recv_data[total_data_len],remain_data_len);
            if (ret < 0)
            {
                perror("Error: serial read error");
                continue;
            }

            total_data_len += ret;
            remain_data_len = (recv_data_len - total_data_len);

            //最后接收到\n或则数据超长时接收成功
            if (total_data_len == recv_data_len || recv_data[total_data_len - 1] == '\n')
            {
                printf("recv ： %s\r\n", recv_data);
                memset(recv_data, 0, sizeof(recv_data));
                total_data_len = 0;
                remain_data_len = 0;
                continue;
            }
        }
    }
}

int main(int argc, char **argv)
{
    int fd = -1;
    int reg;
    fd = uart_open(UART_PATH, UART_BAUD_RATE_115200, UART_DATA_BIT_8, UART_PARITY_BIT_NONE, UART_STOP_BIT_1);
    if (fd < 0)
    {
        printf("Error : OPEN %s fail\r\n", UART_PATH);
        return -1;
    }

    uart_write_buff(fd, "\r\n uart init success: ");
    pthread_t g_pt;

    pthread_create(&g_pt, NULL, uart_recv_pthread, (void *)fd);
    if (g_pt <= 0)
    {
        printf("Error: create check uart  thread failed\n");
        return -1;
    }

#if 1
    char buff[1024];
    while (1)
    {
        printf("send: ");
        scanf("%s", buff);
        uart_write_buff(fd, buff);
    }
#else
    while (1)
    {
        printf("可把进程与后台形式开启( eg: %s & ) ,之后使用top查看cpu占用\r\n", argv[0]);
        sleep(1);
    }
#endif
    return 0;
}
