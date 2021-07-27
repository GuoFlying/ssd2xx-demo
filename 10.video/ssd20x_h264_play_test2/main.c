#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mi_vdec.h"
#include "mi_vdec_datatype.h"
#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_sys_datatype.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"

#include "SAT070CP50_1024x600.h"

#define MAX_ONE_FRM_SIZE (2 * 1024 * 1024)
#define VDEC_CHN_ID 0

MI_U32 g_u32InWidth = 1280;
MI_U32 g_u32InHeight = 720;
MI_U32 g_u32OutWidth = 1024;
MI_U32 g_u32OutHeight = 600;
MI_U32 g_u32RefFrmNum = 2;

#ifndef MAKE_YUYV_VALUE
#define MAKE_YUYV_VALUE(y, u, v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0, 128, 128)
#endif

#define STCHECKRESULT(result)                                            \
    if (result != MI_SUCCESS)                                            \
    {                                                                    \
        printf("[%s %d]exec function failed\n", __FUNCTION__, __LINE__); \
        return 1;                                                        \
    }                                                                    \
    else                                                                 \
    {                                                                    \
        printf("(%s %d)exec function pass\n", __FUNCTION__, __LINE__);   \
    }

typedef struct ReOrderSlice_s
{
    MI_U8 *pos;
    MI_U32 len;
} ReOrderSlice_t;

typedef struct
{
    int startcodeprefix_len;
    unsigned int len;
    unsigned int max_size;
    char *buf;
    unsigned short lost_packets;
} NALU_t;

ReOrderSlice_t _stVdecSlice[100];
MI_U32 _u32SliceIdx = 0;
char _pFileName[256];
pthread_t _pthrPushStream = 0;
MI_VDEC_CodecType_e _eCodecType = E_MI_VDEC_CODEC_TYPE_H264;

static int FindStartCode2(unsigned char *Buf)
{
    if ((Buf[0] != 0) || (Buf[1] != 0) || (Buf[2] != 1))
        return 0;
    else
        return 1;
}

static int FindStartCode3(unsigned char *Buf)
{
    if ((Buf[0] != 0) || (Buf[1] != 0) || (Buf[2] != 0) || (Buf[3] != 1))
        return 0;
    else
        return 1;
}

NALU_t *AllocNALU(int buffersize)
{
    NALU_t *n;
    if ((n = (NALU_t *)calloc(1, sizeof(NALU_t))) == NULL)
    {
        printf("AllocNALU: n");
        exit(0);
    }
    n->max_size = buffersize;
    if ((n->buf = (char *)calloc(buffersize, sizeof(char))) == NULL)
    {
        free(n);
        printf("AllocNALU: n->buf");
        exit(0);
    }
    return n;
}

void FreeNALU(NALU_t *n)
{
    if (n)
    {
        if (n->buf)
        {
            free(n->buf);
            n->buf = NULL;
        }
        free(n);
    }
}

int GetAnnexbNALU(NALU_t *nalu, MI_S32 chn, FILE *fp)
{
    int pos = 0;
    int StartCodeFound, rewind;
    unsigned char *Buf;
    int info2 = 0, info3 = 0;

    if ((Buf = (unsigned char *)calloc(nalu->max_size, sizeof(char))) == NULL)
        printf("GetAnnexbNALU: Could not allocate Buf memory\n");
    nalu->startcodeprefix_len = 3;
    if (3 != fread(Buf, 1, 3, fp))
    {
        free(Buf);
        return 0;
    }
    info2 = FindStartCode2(Buf);
    if (info2 != 1)
    {
        if (1 != fread(Buf + 3, 1, 1, fp))
        {
            free(Buf);
            return 0;
        }
        info3 = FindStartCode3(Buf);
        if (info3 != 1)
        {
            free(Buf);
            return -1;
        }
        else
        {
            pos = 4;
            nalu->startcodeprefix_len = 4;
        }
    }
    else
    {
        nalu->startcodeprefix_len = 3;
        pos = 3;
    }
    StartCodeFound = 0;
    info2 = 0;
    info3 = 0;
    while (!StartCodeFound)
    {
        if (feof(fp))
        {
            nalu->len = (pos - 1) - nalu->startcodeprefix_len;
            memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
            free(Buf);
            fseek(fp, 0, 0);
            return pos - 1;
        }
        Buf[pos++] = fgetc(fp);
        info3 = FindStartCode3(&Buf[pos - 4]);
        if (info3 != 1)
            info2 = FindStartCode2(&Buf[pos - 3]);
        StartCodeFound = (info2 == 1 || info3 == 1);
    }
    rewind = (info3 == 1) ? -4 : -3;
    if (0 != fseek(fp, rewind, SEEK_CUR))
    {
        free(Buf);
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
    }
    nalu->len = (pos + rewind);
    memcpy(nalu->buf, &Buf[0], nalu->len);
    free(Buf);
    return (pos + rewind);
}

MI_U64 GetOSTime(void)
{
    MI_U64 u64CurTime = 0;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    u64CurTime = ((unsigned long long)(tv.tv_sec)) * 1000 + tv.tv_usec / 1000;
    return u64CurTime;
}

void *ST_VdecSendStream(void *args)
{
    MI_VDEC_CHN vdecChn = VDEC_CHN_ID;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_S32 s32TimeOutMs = 20;
    MI_U64 u64Pts = 0;
    MI_U32 u32FpBackLen = 0; // if send stream failed, file pointer back length
    FILE *fReadFile = NULL;
    NALU_t *pstNalu;
    MI_VDEC_VideoStream_t stVdecStream;

    pstNalu = AllocNALU(MAX_ONE_FRM_SIZE);

    fReadFile = fopen(_pFileName, "rb"); //ES
    if (!fReadFile)
    {
        printf("Open %s failed!\n", _pFileName);
        return NULL;
    }

    printf("open %s success, vdec chn:%d\n", _pFileName, vdecChn);
    while (1)
    {
        GetAnnexbNALU(pstNalu, vdecChn, fReadFile);
        stVdecStream.pu8Addr = (MI_U8 *)pstNalu->buf;
        if (9 == pstNalu->len && 0 == *(pstNalu->buf) && 0 == *(pstNalu->buf + 1) 
        && 0 == *(pstNalu->buf + 2) && 1 == *(pstNalu->buf + 3) && 0x68 == *(pstNalu->buf + 4) && 0 == *(pstNalu->buf + pstNalu->len - 1))
        {
            stVdecStream.u32Len = 8;
        }
        else
        {
            stVdecStream.u32Len = pstNalu->len;
        }
        stVdecStream.u64PTS = u64Pts;
        stVdecStream.bEndOfFrame = 1;
        stVdecStream.bEndOfStream = 0;

        u32FpBackLen = stVdecStream.u32Len; //back length
   
        if (MI_SUCCESS != (s32Ret = MI_VDEC_SendStream(vdecChn, &stVdecStream, s32TimeOutMs)))
        {
            //printf("chn[%d]: MI_VDEC_SendStream %d fail, 0x%X\n", vdecChn, stVdecStream.u32Len, s32Ret);
            fseek(fReadFile, -u32FpBackLen, SEEK_CUR);
            usleep(30 * 1000);
        }

    }

    FreeNALU(pstNalu);
    fclose(fReadFile);

    return NULL;
}

//反初始化
static int de_init(void)
{
    //Unbind vdec 2 disp
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;

    stSrcChnPort.eModId = E_MI_MODULE_ID_VDEC;
    stSrcChnPort.u32DevId = 0;
    stSrcChnPort.u32ChnId = 0;
    stSrcChnPort.u32PortId = 0;

    stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stDstChnPort.u32DevId = 0;
    stDstChnPort.u32ChnId = 0;
    stDstChnPort.u32PortId = 0;

    STCHECKRESULT(MI_SYS_UnBindChnPort(&stSrcChnPort, &stDstChnPort));

    STCHECKRESULT(MI_VDEC_StopChn(0));
    STCHECKRESULT(MI_VDEC_DestroyChn(0));

    MI_DISP_DisableInputPort(0, 0);
    MI_DISP_DisableVideoLayer(0);
    MI_DISP_UnBindVideoLayer(0, 0);
    MI_DISP_Disable(0);
    MI_PANEL_DeInit();

    MI_SYS_Exit();
    return 0;
}

//强制退出应用时，关闭对应端口
static void ctrl_c_exit(MI_S32 signo)
{
    if (signo == SIGINT)
    {
        printf("catch Ctrl + C, exit\n");
        de_init();
        exit(0);
    }
}

int main(int argc, char **argv)
{
    //检测退出信号
    struct sigaction sigAction;
    sigAction.sa_handler = ctrl_c_exit;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    sigaction(SIGINT, &sigAction, NULL);

    if (argc < 2)
    {
        printf("exec like: ./prog_cardv_test filename\n");
        exit(0);
    }
    memcpy(_pFileName, argv[1], strlen(argv[1]));

    MI_SYS_Init();

    //初始化视频解码功能
    MI_VDEC_ChnAttr_t stVdecChnAttr;
    MI_VDEC_OutputPortAttr_t stOutputPortAttr;
    MI_VDEC_CHN stVdecChn = VDEC_CHN_ID;
    MI_VDEC_InitParam_t stInitParm;

    memset(&stVdecChnAttr, 0, sizeof(MI_VDEC_ChnAttr_t));
    stVdecChnAttr.eCodecType = E_MI_VDEC_CODEC_TYPE_H264;
    stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = g_u32RefFrmNum; //参考帧个数，会影响内存，推荐2
    stVdecChnAttr.eVideoMode = E_MI_VDEC_VIDEO_MODE_FRAME;         //码流传送方式
    stVdecChnAttr.u32BufSize = 1 * g_u32InWidth * g_u32InHeight;   //缓冲buf大小
    stVdecChnAttr.u32PicWidth = g_u32InWidth;
    stVdecChnAttr.u32PicHeight = g_u32InHeight;
    stVdecChnAttr.eDpbBufMode = E_MI_VDEC_DPB_MODE_NORMAL;
    stVdecChnAttr.u32Priority = 1; //通道优先级

    memset(&stInitParm, 0x0, sizeof(stInitParm));
    stInitParm.bDisableLowLatency = TRUE;
    MI_VDEC_InitDev(&stInitParm);
    STCHECKRESULT(MI_VDEC_CreateChn(stVdecChn, &stVdecChnAttr)); //创建视频解码通道。
    STCHECKRESULT(MI_VDEC_StartChn(stVdecChn));                  //开启解码器

    memset(&stOutputPortAttr, 0, sizeof(MI_VDEC_OutputPortAttr_t));
    stOutputPortAttr.u16Width = g_u32OutWidth;
    stOutputPortAttr.u16Height = g_u32OutHeight;
    STCHECKRESULT(MI_VDEC_SetOutputPortAttr(0, &stOutputPortAttr)); //设置解码通道输出端口属性。

    MI_SYS_ChnPort_t stChnPort;
    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VDEC;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = stVdecChn;
    stChnPort.u32PortId = 0;

    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(&stChnPort, 0, 5)); //设置通道OutputPort的深度

    //init disp
    MI_PANEL_LinkType_e eLinkType;
    MI_DISP_PubAttr_t stDispPubAttr;

    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stDispPubAttr.u32BgColor = YUYV_BLACK;

    stDispPubAttr.stSyncInfo.u16Vact = stPanelParam.u16Height;
    stDispPubAttr.stSyncInfo.u16Vbb = stPanelParam.u16VSyncBackPorch;
    stDispPubAttr.stSyncInfo.u16Vfb = stPanelParam.u16VTotal - (stPanelParam.u16VSyncWidth +
                                                                stPanelParam.u16Height + stPanelParam.u16VSyncBackPorch);
    stDispPubAttr.stSyncInfo.u16Hact = stPanelParam.u16Width;
    stDispPubAttr.stSyncInfo.u16Hbb = stPanelParam.u16HSyncBackPorch;
    stDispPubAttr.stSyncInfo.u16Hfb = stPanelParam.u16HTotal - (stPanelParam.u16HSyncWidth +
                                                                stPanelParam.u16Width + stPanelParam.u16HSyncBackPorch);
    stDispPubAttr.stSyncInfo.u16Bvact = 0;
    stDispPubAttr.stSyncInfo.u16Bvbb = 0;
    stDispPubAttr.stSyncInfo.u16Bvfb = 0;
    stDispPubAttr.stSyncInfo.u16Hpw = stPanelParam.u16HSyncWidth;
    stDispPubAttr.stSyncInfo.u16Vpw = stPanelParam.u16VSyncWidth;
    stDispPubAttr.stSyncInfo.u32FrameRate = stPanelParam.u16DCLK * 1000000 / (stPanelParam.u16HTotal * stPanelParam.u16VTotal);
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;

#if USE_MIPI
    eLinkType = E_MI_PNL_LINK_MIPI_DSI;
#else
    eLinkType = E_MI_PNL_LINK_TTL;
#endif

    MI_DISP_SetPubAttr(0, &stDispPubAttr); //DISP初始化
    MI_DISP_Enable(0);
    MI_DISP_BindVideoLayer(0, 0);
    MI_DISP_EnableVideoLayer(0);

    MI_DISP_InputPortAttr_t stInputPortAttr;
    memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));

    stInputPortAttr.u16SrcWidth = g_u32OutWidth;
    stInputPortAttr.u16SrcHeight = g_u32OutHeight;
    stInputPortAttr.stDispWin.u16X = 0;
    stInputPortAttr.stDispWin.u16Y = 0;
    stInputPortAttr.stDispWin.u16Width = g_u32OutWidth;
    stInputPortAttr.stDispWin.u16Height = g_u32OutHeight;

    MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr); //设置端口输出位置
    MI_DISP_EnableInputPort(0, 0);
    MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN); //设定指定的视频输入端口同步模式。不检查PTS的同步模式

    MI_PANEL_Init(eLinkType); //屏幕初始化
    MI_PANEL_SetPanelParam(&stPanelParam);
    if (eLinkType == E_MI_PNL_LINK_MIPI_DSI)
    {
#if USE_MIPI
        MI_PANEL_SetMipiDsiConfig(&stMipiDsiConfig);
#endif
    }

    //绑定 vdec 和 disp
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;

    stSrcChnPort.eModId = E_MI_MODULE_ID_VDEC;
    stSrcChnPort.u32DevId = 0;
    stSrcChnPort.u32ChnId = 0;
    stSrcChnPort.u32PortId = 0;

    stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
    stDstChnPort.u32DevId = 0;
    stDstChnPort.u32ChnId = 0;
    stDstChnPort.u32PortId = 0;
    MI_SYS_BindChnPort(&stSrcChnPort, &stDstChnPort, 30, 30); //源端口配置的帧率，目标端口配置的帧率

    //读H264数据传给VDEC
    pthread_create(&_pthrPushStream, NULL, ST_VdecSendStream, NULL);

    while (1)
    {
        sleep(1);
    }

    return 0;
}
