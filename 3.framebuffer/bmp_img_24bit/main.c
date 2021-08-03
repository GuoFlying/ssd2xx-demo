#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "fb_info.h"

//14byte文件头
typedef struct
{
    char cfType[2];  //文件类型，"BM"(0x4D42)
    long cfSize;     //文件大小（字节）
    long cfReserved; //保留，值为0
    long cfoffBits;  //数据区相对于文件头的偏移量（字节）
} __attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐

//40byte信息头
typedef struct
{
    char ciSize[4];          //BITMAPFILEHEADER所占的字节数
    long ciWidth;            //宽度
    long ciHeight;           //高度
    char ciPlanes[2];        //目标设备的位平面数，值为1
    int ciBitCount;          //每个像素的位数
    char ciCompress[4];      //压缩说明
    char ciSizeImage[4];     //用字节表示的图像大小，该数据必须是4的倍数
    char ciXPelsPerMeter[4]; //目标设备的水平像素数/米
    char ciYPelsPerMeter[4]; //目标设备的垂直像素数/米
    char ciClrUsed[4];       //位图使用调色板的颜色数
    char ciClrImportant[4];  //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} __attribute__((packed)) PIXEL; //颜色模式RGB

BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;

static char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;

int show_bmp(char *bmpfile);

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("eg: %s ./res/saturation_800x480_24bit.bmp\r\n",argv[0]);
        return -1;
    }

    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;

    //打开显示设备
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    fb_printf_base_info(fbfd);
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error：reading fixed information.\n");
        exit(2);
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error: reading variable information.\n");
        exit(3);
    }

    xres = vinfo.xres;
    yres = vinfo.yres;
    bits_per_pixel = vinfo.bits_per_pixel;

    //计算屏幕的总大小（字节）
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize=%d byte\n", screensize);

    //对象映射
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1)
    {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }

    printf("sizeof file header=%d\n", sizeof(BITMAPFILEHEADER));

    printf("into show_bmp function\n");

    //显示图像
    show_bmp(argv[1]);

    //删除对象映射
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

int show_bmp(char *bmpfile)
{
    FILE *fp;
    int rc;
    int line_x, line_y;
    long int location = 0, BytesPerLine = 0;

    fp = fopen(bmpfile, "rb");
    if (fp == NULL)
    {
        return (-1);
    }

    rc = fread(&FileHead, sizeof(BITMAPFILEHEADER), 1, fp);
    if (rc != 1)
    {
        printf("read header error!\n");
        fclose(fp);
        return (-2);
    }

    //检测是否是bmp图像
    if (memcmp(FileHead.cfType, "BM", 2) != 0)
    {
        printf("it's not a BMP file\n");
        fclose(fp);
        return (-3);
    }

    rc = fread((char *)&InfoHead, sizeof(BITMAPINFOHEADER), 1, fp);
    if (rc != 1)
    {
        printf("read infoheader error!\n");
        fclose(fp);
        return (-4);
    }
    printf("ciBitCount = %d,w = %d,h=%d\r\n", InfoHead.ciBitCount, InfoHead.ciWidth, InfoHead.ciHeight);

    //跳转的数据区
    fseek(fp, FileHead.cfoffBits, SEEK_SET);
    //每行字节数
    BytesPerLine = (InfoHead.ciWidth * InfoHead.ciBitCount + 31) / 32 * 4;

    line_x = line_y = 0;
    //向framebuffer中写BMP图片
    while (!feof(fp))
    {
        PIXEL pix;
        rc = fread((char *)&pix, 1, sizeof(PIXEL), fp);
        if (rc != sizeof(PIXEL))
            break;
        location = line_x * bits_per_pixel / 8 + (InfoHead.ciHeight - line_y - 1) * xres * bits_per_pixel / 8;

        //显示每一个像素 ARGB
        *(fbp + location + 0) = pix.blue;
        *(fbp + location + 1) = pix.green;
        *(fbp + location + 2) = pix.red;
        *(fbp + location + 3) = 0xff;

        line_x++;
        if (line_x == InfoHead.ciWidth)
        {
            line_x = 0;
            line_y++;
            if (line_y == InfoHead.ciHeight)
                break;
        }
    }
    fclose(fp);
    return (0);
}