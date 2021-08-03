#include <mi_sys_datatype.h>
#include <mi_sys.h>
#include <mi_divp_datatype.h>
#include <mi_divp.h>
#include <mi_disp_datatype.h>
#include <mi_disp.h>
#include <mi_gfx.h>
#include <mi_gfx_datatype.h>
#include <mi_vdisp.h>
#include <mi_vdisp_datatype.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include "sstardisp.h"
#include "blitutil.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct IMAGE
{
    unsigned long long phyAddr;
    char *virtAddr;
    unsigned short width;
    unsigned short height;
    unsigned int pitch;
} IMAGE;

int main(int argc, char *argv[])
{
    if(argc < 4){
        printf("eg: %s ./res/YUV420SP_1280_720_NV12_img.yuv 1280 720\r\n",argv[0]);
        return -1;
    }

    MI_DISP_PubAttr_t stDispPubAttr;
    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    //sys_init ,panel init
    sstar_disp_init(&stDispPubAttr);

    MI_GFX_Open();
    Rect dispWin = {0, 0, atoi(argv[2]), atoi(argv[3])};
    Rect dispOutWin = {0, 0, 1024, 600};
    IMAGE image;
    image.height = dispWin.h;
    image.width = dispWin.w;
    image.pitch = image.width;

    //MMA内存申请
    if (MI_SUCCESS != MI_SYS_MMA_Alloc((MI_U8 *)"#rotat420",dispWin.w * dispWin.h * 3 / 2, &image.phyAddr))
    {
        printf("MI_SYS_MMA_Alloc");
    }
    if (MI_SUCCESS != MI_SYS_Mmap(image.phyAddr,dispWin.w * dispWin.h * 3 / 2, (void **)&image.virtAddr, FALSE))
    {
        printf("MI_SYS_Mmap");
    }
    
    //读取yuv图片信息
    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL){
        printf("fopen error\r\n");
        return -1;
    }
    if (fread(image.virtAddr, dispWin.w * dispWin.h * 3 / 2, 1, fp) < 1)
    {
        printf("reading input file\r\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);

    
    MI_DIVP_ChnAttr_t stAttr;
    memset(&stAttr, 0, sizeof(MI_DIVP_ChnAttr_t));

    MI_DIVP_CreateChn(0, &stAttr);
    MI_DIVP_SetChnAttr(0, &stAttr);
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;
    memset(&stOutputPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
    stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stOutputPortAttr.u32Height = dispOutWin.h;
    stOutputPortAttr.u32Width = (dispOutWin.w);
    stOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
    MI_DIVP_SetOutputPortAttr(0, &stOutputPortAttr);
    MI_DIVP_StartChn(0);

    MI_DISP_EnableVideoLayer(1);
    MI_DISP_InputPortAttr_t stInputPortAttr;
    stInputPortAttr.stDispWin.u16X = dispOutWin.x;
    stInputPortAttr.stDispWin.u16Y = dispOutWin.y;
    stInputPortAttr.stDispWin.u16Width = dispOutWin.w;
    stInputPortAttr.stDispWin.u16Height = dispOutWin.h;
    stInputPortAttr.u16SrcHeight = dispOutWin.h;
    stInputPortAttr.u16SrcWidth = (dispOutWin.w);
    MI_DISP_SetInputPortAttr(1, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(1, 0);

    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;

    stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stSrcChnPort.u32ChnId = 0;
    stSrcChnPort.u32DevId = 0;
    stSrcChnPort.u32PortId = 0;
    stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stDstChnPort.u32ChnId = 0;
    stDstChnPort.u32DevId = 0;
    stDstChnPort.u32PortId = 0;
    MI_SYS_BindChnPort(&stSrcChnPort, &stDstChnPort, 30, 30);

    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE bufHandle;
    memset(&stBufConf, 0, sizeof(MI_SYS_BufConf_t));
    stBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stBufConf.stFrameCfg.u16Height = image.height;
    stBufConf.stFrameCfg.u16Width = image.width;
    printf("% d %d\n", image.width, image.height);
    stBufConf.u32Flags = MI_SYS_MAP_VA;

    int keepRunning = 1;
    struct timeval start, end;
    long long total_time;
    gettimeofday(&start, NULL);
   
    while (keepRunning)
    {
        keepRunning ++;
        gettimeofday(&start, NULL);
        
        MI_SYS_ChnInputPortGetBuf(&stSrcChnPort, &stBufConf, &stBufInfo, &bufHandle, 0);

        if (1)
        {
#if 0
            memcpy(stBufInfo.stFrameData.pVirAddr[0], image.virtAddr, dispWin.w * dispWin.h * 3 / 2);
#else
            Surface srcY, dstY;
            Surface srcUV, dstUV;
            RECT r;
            //Y 翻转
            srcY.eGFXcolorFmt = E_MI_GFX_FMT_I8;
            srcY.h = image.height;
            srcY.phy_addr = image.phyAddr;
            srcY.pitch = image.pitch;
            srcY.w = image.width;
            srcY.BytesPerPixel = 1;

            dstY.eGFXcolorFmt = E_MI_GFX_FMT_I8;
            dstY.h = srcY.h;
            dstY.phy_addr = stBufInfo.stFrameData.phyAddr[0];
            dstY.pitch = srcY.w;
            dstY.w = srcY.w;
            dstY.BytesPerPixel = 1;
            r.left = 0;
            r.top = 0;
            r.bottom = srcY.h;
            r.right = srcY.w;
            if(keepRunning % 4 == 0){
                SstarBlitCCW(&srcY, &dstY, &r);
            }else if(keepRunning % 4 == 1){
                SstarBlitNormal(&srcY, &dstY, &r);
            }else if(keepRunning % 4 == 2){
                SstarBlitHVFlip(&srcY, &dstY, &r);
            }else {
                SstarBlitCW(&srcY, &dstY, &r);
            }

            //UV翻转
            srcUV.eGFXcolorFmt = E_MI_GFX_FMT_ARGB8888;
            srcUV.h = image.height / 2;
            srcUV.phy_addr = image.phyAddr + image.width * image.height;
            srcUV.pitch = image.width;
            srcUV.w = image.width / 2;
            srcUV.BytesPerPixel = 2;

            dstUV.eGFXcolorFmt = E_MI_GFX_FMT_ARGB8888;
            dstUV.h = srcUV.h;
            dstUV.phy_addr = stBufInfo.stFrameData.phyAddr[1];
            dstUV.pitch = srcUV.w * 2;
            dstUV.w = srcUV.w;
            dstUV.BytesPerPixel = 2;
            r.left = 0;
            r.top = 0;
            r.bottom = srcUV.h;
            r.right = srcUV.w;
            if(keepRunning % 4 == 0){
                SstarBlitCCW(&srcUV, &dstUV, &r);
            }else if(keepRunning % 4 == 1){
                SstarBlitNormal(&srcUV, &dstUV, &r);
            }else if(keepRunning % 4 == 2){
                SstarBlitHVFlip(&srcUV, &dstUV, &r);
            }else{
                SstarBlitCW(&srcUV, &dstUV, &r);
            }
#endif
        }
        MI_SYS_ChnInputPortPutBuf(bufHandle, &stBufInfo, FALSE);

        gettimeofday(&end, NULL);
        total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
        printf("%s %d : total time is %lld us\n",__func__,__LINE__, total_time);
        sleep(3);
    }
    MI_SYS_UnBindChnPort(&stSrcChnPort, &stDstChnPort);
    MI_DIVP_StopChn(0);
    MI_DIVP_DestroyChn(0);
    MI_DISP_DisableInputPort(1, 0);
    MI_DISP_DisableVideoLayer(1);
    sstar_disp_Deinit(&stDispPubAttr);

    return 0;
}
