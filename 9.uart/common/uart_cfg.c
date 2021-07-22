#include "uart_cfg.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <pthread.h>

int uart_open(const char *uart_name,
              const UART_BAUD_E baud_rate,
              const UART_DATA_BIT_E data_bit,
              const UART_PARITY_BIT_E parity_bit,
              const UART_STOP_BIT_E stop_bit)
{
    int fd = -1;
    int ret = -1;
    struct termios option = {0}; // 串口属性结构体

    // 打开串口
    fd = open(uart_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        return -1;
    }

    // 获取终端属性
    tcgetattr(fd, &option);

    // 设置输入波特率
    cfsetspeed(&option, baud_rate);
    // 设置输出波特率
    cfsetospeed(&option, baud_rate);

    // 设置数据位
    option.c_cflag &= ~CSIZE;
    option.c_cflag |= data_bit;

    // 设置奇偶检验位
    switch (parity_bit)
    {
    // 无校验
    case UART_PARITY_BIT_NONE:
    {
        option.c_cflag &= ~PARENB;
        option.c_iflag &= ~INPCK;

        break;
    }

    // 奇校验
    case UART_PARITY_BIT_ODD:
    {
        option.c_cflag |= (PARODD | PARENB);
        option.c_iflag |= INPCK;

        break;
    }

    // 偶校验
    case UART_PARITY_BIT_EVEN:
    {
        option.c_cflag |= PARENB;
        option.c_cflag &= ~PARODD;
        option.c_iflag |= INPCK;

        break;
    }

    // 默认为无校验
    default:
    {
        option.c_cflag &= ~PARENB;
        option.c_iflag &= ~INPCK;

        break;
    }
    }

    // 设置停止位
    switch (stop_bit)
    {
    // 1位停止位
    case UART_STOP_BIT_1:
    {
        option.c_cflag &= ~CSTOPB;
        break;
    }

    // 2位停止位
    case UART_STOP_BIT_2:
    {
        option.c_cflag |= CSTOPB;
        break;
    }

    // 默认为1位停止位
    default:
    {
        option.c_cflag &= ~CSTOPB;
        break;
    }
    }

    // 一般必设置的标志
    option.c_cflag |= (CLOCAL | CREAD);
    option.c_oflag &= ~(OPOST);
    option.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    option.c_iflag &= ~(ICRNL | INLCR | IGNCR | IXON | IXOFF | IXANY);

    // 清空输入输出缓冲区
    tcflush(fd, TCIOFLUSH);

    // 设置最小接收字符数和超时时间
    // 当MIN=0, TIME=0时, 如果有数据可用, 则read最多返回所要求的字节数,
    // 如果无数据可用, 则read立即返回0
    option.c_cc[VMIN] = 0;
    option.c_cc[VTIME] = 0;

    // 设置终端属性
    ret = tcsetattr(fd, TCSANOW, &option);
    if (ret < 0)
    {
        close(fd);

        return -1;
    }

    return fd;
}

//关闭串口
int close_uart(const int fd)
{
    if (fd < 0)
    {
        return -1;
    }

    return close(fd);
}

//清空串口输入缓存
int uart_flush_input_data(const int fd)
{
    if (fd < 0)
    {
        return -1;
    }

    return tcflush(fd, TCIFLUSH);
}

//清空串口输出缓存
int uart_flush_output_data(const int fd)
{
    if (fd < 0)
    {
        return -1;
    }

    return tcflush(fd, TCOFLUSH);
}

//清空串口输入和输出缓存
int uart_flush_both_data(const int fd)
{
    if (fd < 0)
    {
        return -1;
    }

    return tcflush(fd, TCIOFLUSH);
}

//串口发送数据
int uart_write_data(const int fd, const uint8_t *send_data,
                    const uint32_t len)
{
    int ret;
    if (fd < 0 || !send_data)
    {
        return -1;
    }
    ret = write(fd, send_data, len);
    if (len != ret)
    {
        return -1;
    }
    return 0;
}

int uart_write_buff(const int fd, const char *buff)
{
    int ret;
    int len =  strlen(buff);
    if (fd < 0  || !buff)
    {
        return -1;
    }
    ret = write(fd, buff, len);
    if (len != ret)
    {
        return -1;
    }
    return 0;
}

//串口接收数据

int uart_read_data(const int fd, uint8_t *recv_data, const uint32_t len)
{
    int ret;
    if (fd < 0 || !recv_data)
    {
        return -1;
    }
    ret = read(fd, recv_data, len);
    if (ret < 0)
    {
        return -1;
    }
    return ret;
}
