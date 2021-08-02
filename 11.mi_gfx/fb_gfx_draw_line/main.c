#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "mi_gfx.h"
#include "mi_sys.h"
#include <sys/time.h>

int main(int argc, char **argv)
{
    MI_SYS_Init();
    MI_GFX_Open();

    int fd = 0;
    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0)
    {
        printf("error: can not open framebuffer device/r/n");
        exit(1);
    }

    struct fb_fix_screeninfo finfo;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information/n");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    //获取屏幕可变信息
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information/n");
        return -1;
    }

    MI_GFX_Surface_t pstDst;
    MI_U16 fence;

    MI_GFX_Line_t pstLine;

    pstDst.eColorFmt = E_MI_GFX_FMT_ARGB8888;
    pstDst.u32Width = vinfo.xres;
    pstDst.u32Height = vinfo.yres;
    pstDst.u32Stride = vinfo.xres * (vinfo.bits_per_pixel / 8);
    pstDst.phyAddr = (MI_PHY)finfo.smem_start;

    //开始坐标
    pstLine.stPointFrom.s16x = 0;
    pstLine.stPointFrom.s16y = 0;
    //结束坐标
    pstLine.stPointTo.s16x = vinfo.xres;
    pstLine.stPointTo.s16y = vinfo.yres;
    //线像素
    pstLine.u16Width = 10;
    //是否渐变色
    pstLine.bColorGradient = TRUE;
    //颜色
    pstLine.u32ColorFrom = 0xffff0000;
    //渐变到什么颜色
    pstLine.u32ColorTo = 0xff0000ff;  

    struct timeval start, end;
    long long total_time;
    gettimeofday(&start, NULL);

    MI_GFX_DrawLine(&pstDst, &pstLine, &fence);
    MI_GFX_WaitAllDone(FALSE, fence);
    
    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("%s %d : total time is %lld us\n",__func__,__LINE__, total_time);

     //开始坐标
    pstLine.stPointFrom.s16x = 0;
    pstLine.stPointFrom.s16y = vinfo.yres;
    //结束坐标
    pstLine.stPointTo.s16x = vinfo.xres;
    pstLine.stPointTo.s16y = 0;
    //线像素
    pstLine.u16Width = 1;
    //固定色
    pstLine.bColorGradient = FALSE;
    //颜色
    pstLine.u32ColorFrom = 0xff00ff00;

    MI_GFX_DrawLine(&pstDst, &pstLine, &fence);
    MI_GFX_WaitAllDone(FALSE, fence);

    fprintf(stderr, "vinfo.xres y = %d %d/n", vinfo.xres, vinfo.yres);

    sleep(1);

    MI_GFX_Close();
    MI_SYS_Exit();

    return 0;
}
