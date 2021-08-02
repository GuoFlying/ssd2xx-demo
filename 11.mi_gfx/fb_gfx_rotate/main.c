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

// int __test_rotate_ARGB_90(MI_GFX_Surface_t srcSurf, MI_GFX_Rect_t srcRect,
//                           MI_GFX_Surface_t dstSurf, MI_GFX_Rect_t dstRect, char *dstData, MI_BOOL bSinkSurf)
// {
//     Surface src, dst;
//     RECT srcClip;
//     dstSurf.eColorFmt = srcSurf.eColorFmt;
//     dstSurf.u32Stride = dstSurf.u32Width*getBpp(dstSurf.eColorFmt);

//     dstSurf.u32Height = srcSurf.u32Width;
//     dstSurf.u32Width = srcSurf.u32Height;
//     dstSurf.u32Stride = dstSurf.u32Width * getBpp(dstSurf.eColorFmt);
    
//     src.eGFXcolorFmt = srcSurf.eColorFmt;
//     src.BytesPerPixel = getBpp(srcSurf.eColorFmt);
//     src.h = srcSurf.u32Height;
//     src.w = srcSurf.u32Width;
//     src.pitch = src.w * src.BytesPerPixel;
//     src.phy_addr = srcSurf.phyAddr;

//     dst.eGFXcolorFmt = dstSurf.eColorFmt;
//     dst.BytesPerPixel = getBpp(dstSurf.eColorFmt);
//     dst.h = dstSurf.u32Height;
//     dst.w = dstSurf.u32Width;
//     dst.pitch = dst.w * dst.BytesPerPixel;
//     dst.phy_addr = dstSurf.phyAddr;

//     srcClip.left = 0;
//     srcClip.top = 0;
//     srcClip.bottom = src.h;
//     srcClip.right = src.w;
//     SstarBlitCW(&src, &dst, &srcClip);

//     return 0;
// }

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

    struct timeval start, end;
    long long total_time;
    gettimeofday(&start, NULL);

    MI_GFX_QuickFill(&pstDst, &pstDstRect, u32ColorVal, &fence);
    MI_GFX_WaitAllDone(FALSE, fence);

    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("%s %d : total time is %lld us\n",__func__,__LINE__, total_time);

    pstDstRect.s32Xpos = 0;
    pstDstRect.s32Ypos = 0;
    pstDstRect.u32Height = 200;
    pstDstRect.u32Width = 200;
    u32ColorVal = 0xffff0000;

    gettimeofday(&start, NULL);

    MI_GFX_QuickFill(&pstDst, &pstDstRect, u32ColorVal, &fence);
    MI_GFX_WaitAllDone(FALSE, fence);
    
    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("%s %d : total time is %lld us\n",__func__,__LINE__, total_time);

    fprintf(stderr, "vinfo.xres y = %d %d/n", vinfo.xres, vinfo.yres);

    sleep(1);
    MI_GFX_Close();
    MI_SYS_Exit();

    return 0;
}
