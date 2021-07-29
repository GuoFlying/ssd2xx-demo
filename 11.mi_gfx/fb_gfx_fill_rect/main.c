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

int main(int argc, char **argv)
{
    MI_GFX_Surface_t pstDst;
    MI_GFX_Rect_t pstDstRect;
    MI_U32 u32ColorVal;
    MI_U16 fence;
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

    pstDst.eColorFmt = E_MI_GFX_FMT_ARGB8888;
    pstDst.u32Width = vinfo.xres;
    pstDst.u32Height = vinfo.yres;
    pstDst.u32Stride = vinfo.xres * (vinfo.bits_per_pixel / 8);
    pstDst.phyAddr = (MI_PHY)finfo.smem_start;

    pstDstRect.s32Xpos = 0;
    pstDstRect.s32Ypos = 0;
    pstDstRect.u32Height = vinfo.yres;
    pstDstRect.u32Width = vinfo.xres;

    u32ColorVal = 0xffff00ff;

    MI_GFX_QuickFill(&pstDst, &pstDstRect, u32ColorVal, &fence);
    MI_GFX_WaitAllDone(FALSE, fence);

    pstDstRect.s32Xpos = 0;
    pstDstRect.s32Ypos = 0;
    pstDstRect.u32Height = 200;
    pstDstRect.u32Width = 200;
    u32ColorVal = 0xffff0000;

    MI_GFX_QuickFill(&pstDst, &pstDstRect, u32ColorVal, &fence);
    MI_GFX_WaitAllDone(FALSE, fence);

    fprintf(stderr, "vinfo.xres y = %d %d/n", vinfo.xres, vinfo.yres);
    MI_GFX_Close();
    MI_SYS_Exit();

    return 0;
}
