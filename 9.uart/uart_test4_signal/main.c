
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include "uart_cfg.h"

#define UART_PATH "/dev/ttyS1"

static int flag = 0;
//中断函数不要做过多的事情
static void uart_signal_handler(int status)
{
    printf("received SIGIO signale.\n");
    flag = 1;
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

    uart_write_data(fd, "\r\n uart init success: ", sizeof("\r\n uart init success: "));

    struct sigaction saio;
    saio.sa_handler = uart_signal_handler;
    sigemptyset(&saio.sa_mask);
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO, &saio, NULL);

    //allow the process to receive SIGIO
    fcntl(fd, F_SETOWN, getpid());
    //make the file descriptor asynchronous
    fcntl(fd, F_SETFL, FASYNC);
    uint8_t data[1024];
    int res = 0;
    while (1)
    {
        sleep(-1); //永久sleep。但当触发中断时候，会唤醒sleep
        if (flag == 1)
        {
            memset(data, 0, sizeof(data));
            res = uart_read_data(fd, data, 1024);

            uart_write_buff(fd,"recv: ");
            uart_write_buff(fd,(char *)data);
            uart_write_buff(fd,"\r\n");

            printf("recv = %d , %s\n", res, data);
            flag = 0; 
        }
    }
    return 0;
}
