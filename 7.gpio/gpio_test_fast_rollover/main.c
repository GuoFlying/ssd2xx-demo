#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for usleep
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <libgen.h>
#define USE_FASE 1

#define BANK 0x103C
#define OFFSET 0x0b
#define CONTENT_H 0x0011
#define CONTENT_L 0x0000

/*copy to kernel/drivers/sstar/include*/
#define MSYS_IOCTL_MAGIC 'S'
#define IOCTL_MSYS_GET_RIU_MAP _IO(MSYS_IOCTL_MAGIC, 0x15)

typedef struct
{
    unsigned int VerChk_Version;
    unsigned long long addr;
    unsigned int size;
    unsigned int VerChk_Size;
} __attribute__((__packed__)) MSYS_MMIO_INFO;

#define IOCTL_MSYS_VERSION    0x0100
#define VERCHK_HEADER       0x4D530000
#define VERCHK_HADER_MASK   0xFFFF0000
#define VERCHK_VERSION_MASK 0x0000FFFF
#define VERCHK_MAJORVERSION_MASK 0x0000FF00

#define FILL_VERCHK_TYPE(var, var_ver, var_size, version)        \
({                                                               \
    var_ver = (VERCHK_HEADER | (version & VERCHK_VERSION_MASK)); \
    var_size = sizeof(var);                                      \
    var;                                                         \
})

int main(int argc, char **argv)
{

#if USE_FASE
    unsigned char *map_base = NULL;
    unsigned long addr;
    int mem_fd = 0;
    int msys_fd = 0;
    MSYS_MMIO_INFO info;

    int i = 0;
    unsigned long bank = 0, offset = 0;

    FILL_VERCHK_TYPE(info, info.VerChk_Version, info.VerChk_Size, IOCTL_MSYS_VERSION);

    msys_fd = open("/dev/msys", O_RDWR | O_SYNC);
    if (-1 == msys_fd)
    {
        printf("can't open /dev/msys\n");
        return -1;
    }
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (-1 == mem_fd)
    {
        printf("can't open /dev/mem\n");
        return -1;
    }

    if (0 != ioctl(msys_fd, IOCTL_MSYS_GET_RIU_MAP, &info))
    {
        printf("DMEM request failed!!\n");
        return -1;
    }

    map_base = mmap(NULL, info.size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, info.addr);
    if (map_base == 0)
    {
        printf("NULL pointer!\n");
        return -1;
    }

    addr = (unsigned long)(map_base + BANK * 0x200 + OFFSET * 4);
    //测试频率可达21.6M Hz
    while (1)
    {
        *(unsigned short *)addr = CONTENT_L;
        //usleep(1000); //没开高精度时钟时usleep最小时间为10ms，若要通过usleep来控制翻转频率，则需要打开高精度时钟
        *(unsigned short *)addr = CONTENT_H;
        //usleep(1000);

    }

#else
    system("echo 11 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio11/direction");
    //测试频率只可到172Hz
    while (1)
    {
        system("echo 1 > /sys/class/gpio/gpio11/value");
        system("echo 0 > /sys/class/gpio/gpio11/value");
    }
    return 0;
#endif
}
