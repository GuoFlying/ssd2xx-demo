#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "uart_cfg.h"

#define UART_PATH "/dev/ttyS1"

int main(int argc, char **argv)
{
    int fd = -1;
    int reg;
    fd = uart_open(UART_PATH,UART_BAUD_RATE_115200,UART_DATA_BIT_8,UART_PARITY_BIT_NONE,UART_STOP_BIT_1);
    if(fd < 0){
        printf("Error : OPEN %s fail\r\n",UART_PATH);
        return -1;
    }

    uart_write_buff(fd,"\r\n uart init success: ");
    char data[1024];
    while(1){
        memset(data,0,sizeof(data));
        //非阻塞
        reg = uart_read_data(fd,data,1024);
        if(reg > 0){
            data[reg] = '\0';
            printf("got : %s\n", data);
            uart_write_buff(fd,"\r\n recv: ");
            uart_write_data(fd,data,reg);
        }
      
    }
    return 0;
}
