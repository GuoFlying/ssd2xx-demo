#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_sys_datatype.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"

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


#define MAKE_YUYV_VALUE(y, u, v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0, 128, 128)
#define YUYV_WHITE MAKE_YUYV_VALUE(255, 128, 128)
#define YUYV_RED MAKE_YUYV_VALUE(76, 84, 255)
#define YUYV_GREEN MAKE_YUYV_VALUE(149, 43, 21)
#define YUYV_BLUE MAKE_YUYV_VALUE(29, 225, 107)

static unsigned long u32DispInWidth;
static unsigned long u32DispInHeight;

static unsigned long u32DispOutWidth;
static unsigned long u32DispOutHeight;

static int ST_Screen_Init(void)
{
	MI_DISP_PubAttr_t stPubAttr;
	MI_DISP_VideoLayerAttr_t stLayerAttr;
	MI_DISP_InputPortAttr_t stInputPortAttr;
	MI_PANEL_IntfType_e eIntfType;

	//config disp
	memset(&stPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
	//stPubAttr.u32BgColor = YUYV_BLACK;
	stPubAttr.u32BgColor = 0;   //设定背景颜色
	stPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER; //定义 DISP timing类型。
	stPubAttr.eIntfType = E_MI_DISP_INTF_TTL; //定义视频输出接口
	MI_DISP_SetPubAttr(0, &stPubAttr);   //配置视频输出设备的公共属性
	STCHECKRESULT(MI_DISP_Enable(0));    //启用视频输出设备

	memset(&stPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
	stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
	MI_DISP_GetPubAttr(0, &stPubAttr);  //获取视频设备的公共属性

	memset(&stLayerAttr, 0, sizeof(MI_DISP_VideoLayerAttr_t));

	stLayerAttr.stVidLayerDispWin.u16X = 0; //水平方向起点
	stLayerAttr.stVidLayerDispWin.u16Y = 0; //竖直方向起点
	stLayerAttr.stVidLayerDispWin.u16Width = stPubAttr.stSyncInfo.u16Hact; //水平方向大小
	stLayerAttr.stVidLayerDispWin.u16Height = stPubAttr.stSyncInfo.u16Vact; //竖直方向大小
	STCHECKRESULT(MI_DISP_BindVideoLayer(0, 0)); //绑定视频层到指定设备
	STCHECKRESULT(MI_DISP_SetVideoLayerAttr(0, &stLayerAttr)); //设定视频层属性
	STCHECKRESULT(MI_DISP_EnableVideoLayer(0)); //使能视频层

	memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
	stInputPortAttr.u16SrcWidth = u32DispInWidth; //原yuv宽高
	stInputPortAttr.u16SrcHeight = u32DispInHeight;
	stInputPortAttr.stDispWin.u16X = 0; //显示的坐标原点
	stInputPortAttr.stDispWin.u16Y = 0;
	stInputPortAttr.stDispWin.u16Width = u32DispOutWidth; //显示的宽高
	stInputPortAttr.stDispWin.u16Height = u32DispOutHeight;
	STCHECKRESULT(MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr)); //配置指定视频输入端口的属性。
	STCHECKRESULT(MI_DISP_EnableInputPort(0, 0)); //使能端口

	//init panel
	eIntfType = E_MI_PNL_INTF_TTL;
	STCHECKRESULT(MI_PANEL_Init(eIntfType)); //panel 初始化

	return 0;
}

static int ST_Screen_DeInit(void)
{
	//disable 顺序不能乱
	STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
	STCHECKRESULT(MI_DISP_DisableVideoLayer(0));
	STCHECKRESULT(MI_DISP_UnBindVideoLayer(0, 0));
	STCHECKRESULT(MI_DISP_Disable(0));
	STCHECKRESULT(MI_PANEL_DeInit());

	return 0;
}

static MI_BOOL GetInputFrameDataYuv(int srcFd, MI_SYS_BufInfo_t *pstBufInfo)
{
	MI_BOOL bRet = FALSE;
	MI_U32 u32LineNum = 0;
	MI_U32 u32Index = 0;

	if (E_MI_SYS_PIXEL_FRAME_YUV422_YUYV == pstBufInfo->stFrameData.ePixelFormat)
	{
		MI_U32 u32LineOffset = pstBufInfo->stFrameData.u32Stride[0];
		MI_U32 u32ImageW = u32DispInWidth;
		MI_U32 u32ImageH = u32DispInHeight;
		MI_U32 u32ImageLineCnt = u32ImageH;
		MI_U32 u32ImageBytesPerLine = u32ImageW * 2;
		MI_U32 u32ImageSize = u32ImageBytesPerLine * u32ImageLineCnt;
		MI_U32 u32YUVReadSize = 0;

	READ_YUV422_FILE:
		for (u32Index = 0; u32Index < u32LineNum; u32Index++)
		{
			u32YUVReadSize += read(srcFd, pstBufInfo->stFrameData.pVirAddr[0] + u32Index * u32LineOffset, u32ImageBytesPerLine);
		}
		if (u32YUVReadSize == u32ImageSize)
		{
			bRet = TRUE;
		}
		else if (u32YUVReadSize < u32ImageSize)
		{
			lseek(srcFd, 0, SEEK_SET);
			u32YUVReadSize = 0;
			goto READ_YUV422_FILE;
		}
	}
	else if (E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420 == pstBufInfo->stFrameData.ePixelFormat)
	{
		MI_U32 u32LineOffset = pstBufInfo->stFrameData.u32Stride[0];
		MI_U32 u32ImageW = u32DispInWidth;
		MI_U32 u32ImageH = u32DispInHeight;
		MI_U32 u32ImageYLineCnt = u32ImageH;
		MI_U32 u32ImageUVLineCnt = u32ImageH / 2;
		MI_U32 u32ImageBytesPerLine = u32ImageW;
		MI_U32 u32ImageSize = u32ImageBytesPerLine * (u32ImageYLineCnt + u32ImageUVLineCnt);
		MI_U32 u32YReadSize = 0;
		MI_U32 u32UVReadSize = 0;

	READ_YUV420_FILE:
		for (u32Index = 0; u32Index < u32ImageYLineCnt; u32Index++)
		{
			u32YReadSize += read(srcFd, pstBufInfo->stFrameData.pVirAddr[0] + u32Index * u32LineOffset, u32ImageBytesPerLine);
		}

		for (u32Index = 0; u32Index < u32ImageUVLineCnt; u32Index++)
		{
			u32UVReadSize += read(srcFd, pstBufInfo->stFrameData.pVirAddr[1] + u32Index * u32LineOffset, u32ImageBytesPerLine);
		}

		if (u32YReadSize + u32UVReadSize == u32ImageSize)
		{
			bRet = TRUE;
		}
		else if (u32YReadSize + u32UVReadSize < u32ImageSize)
		{
			lseek(srcFd, 0, SEEK_SET);
			u32YReadSize = 0;
			u32UVReadSize = 0;
			goto READ_YUV420_FILE;
		}
	}

	return bRet;
}

int main(int argc, char **argv)
{
	const char *filename;
	int src_fd;
	MI_SYS_BufConf_t stBufConf;
	MI_SYS_ChnPort_t pstSysChnPort;
	MI_SYS_BufInfo_t stBufInfo;
	MI_SYS_BUF_HANDLE hHandle;
	char c;

	if (argc < 6)
	{
		printf("usage: %s yuv_filename dispinwidth dispinheight dispoutwidth dispoutheight\n",argv[0]);
		return 1;
	}

	filename = argv[1];

	u32DispInWidth = atoi(argv[2]);
	u32DispInHeight = atoi(argv[3]);
	u32DispOutWidth = atoi(argv[4]);
	u32DispOutHeight = atoi(argv[5]);
	//制度打开文件
	src_fd = open(filename, O_RDONLY);
	if (src_fd < 0)
	{
		printf("src_file: %s.\n", filename);
		return -1;
	}

	memset(&stBufConf, 0, sizeof(MI_SYS_BufConf_t));
	memset(&pstSysChnPort, 0, sizeof(MI_SYS_ChnPort_t));
	memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));

	//通道属性设置
	pstSysChnPort.eModId = E_MI_MODULE_ID_DISP; //模块ID枚举类型
	pstSysChnPort.u32ChnId = 0;
	pstSysChnPort.u32DevId = 0;
	pstSysChnPort.u32PortId = 0;

	//使用MI接口必须先初始化SYS
	STCHECKRESULT(MI_SYS_Init());
	
	//屏幕初始化
	ST_Screen_Init();

	while (1)
	{
		stBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
		//stBufConf.u64TargetPts = stTv.tv_sec*1000000 + stTv.tv_usec;
		stBufConf.stFrameCfg.u16Width = u32DispInWidth;
		stBufConf.stFrameCfg.u16Height = u32DispInHeight;
		stBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
		
		//分配通道input端口对应的buf 
		if (MI_SUCCESS == MI_SYS_ChnInputPortGetBuf(&pstSysChnPort, &stBufConf, &stBufInfo, &hHandle, -1))
		{
			stBufInfo.stFrameData.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;  //压缩格式 -- 没有压缩
			stBufInfo.stFrameData.eFieldType = E_MI_SYS_FIELDTYPE_NONE;			//flie 类型 -- 无类型
			stBufInfo.stFrameData.eTileMode = E_MI_SYS_FRAME_TILE_MODE_NONE;	//Tile模式  -- 无模式
			stBufInfo.bEndOfStream = FALSE;//是否发完全部信息
			
			//从文件中读取Yuv数据
			if (GetInputFrameDataYuv(src_fd, &stBufInfo) == TRUE)
			{
				//把通道input端口对应的buf object加到待处理队列,会自动显示
				STCHECKRESULT(MI_SYS_ChnInputPortPutBuf(hHandle, &stBufInfo, FALSE));
			}
			else
			{
				//把通道input端口对应的buf object加到待处理队列 -- 直接放弃对buf的修改不提交，需要释放一些资源
				STCHECKRESULT(MI_SYS_ChnInputPortPutBuf(hHandle, &stBufInfo, TRUE));
				printf("get yuv frame fail\n");
			}
		}
		else
		{
			printf("Get Disp InputBuf Fail\n");
		}
		//usleep(30*1000);
		printf("put q to exit\n");
		c = getchar();
		if (c == 'q')
			break;
	}
	ST_Screen_DeInit();
	close(src_fd);

	return 0;
}
