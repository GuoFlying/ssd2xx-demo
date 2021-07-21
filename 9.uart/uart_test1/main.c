#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    //命令行方式调试串口
    //配置 /dev/ttyS1 串口信息115200/8/N/O
    system("stty -F /dev/ttyS1 ispeed 115200 ospeed 115200 cs8");

    for (int i = 0; i < 5; i++)
    {
        char buff[100];
        sprintf(buff, "echo \"send :%d\" >  /dev/ttyS1", i);
        system(buff);
        usleep(500 * 1000);
    }

    //配置串口，测试发送接收
    int fd, ret;

    fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        perror("open ttyS1");
        return 1;
    }

    char data[1024];

    while (1)
    {
        ret = read(fd, data, sizeof(data));
        if (ret > 0)
        {
            data[ret] = 0;
            printf("got : %s\n", data);
        }
        memset(data, 0, sizeof(data));
        //sleep(1);
    }
    close(fd);
    return 0;
}
