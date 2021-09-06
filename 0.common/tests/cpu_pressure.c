#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/time.h>
#include <pthread.h>
static void cpu_pressure_test()
{
    int fd;
    fd = open("/dev/null", O_RDWR | O_CREAT, 0644);
    if (fd < 0)
    {
        printf("error: open /dev/null\r\n");
        return -1;
    }
    char buf[2048];
    while (1)
    {
        write(fd, buf, sizeof(buf)); //不加sleep，cpu就会基本完全占用，单线程占用一核（cpu最大达50%），两个线程以上会进行多核调度（cpu可达100%）
        //usleep(1);  //如果加个sleep，top看cpu占用就基本为0
    }
}

static void *hand_test(void *pdata)
{
    cpu_pressure_test();
}

int main(int argc, char *argv[])
{

    pthread_t p_init_id;
    pthread_create(&p_init_id, NULL, hand_test, NULL);
    if (!p_init_id)
    {
        printf("Error: create   thread failed\n");
        return -1;
    }

    cpu_pressure_test();

    return 0;
}