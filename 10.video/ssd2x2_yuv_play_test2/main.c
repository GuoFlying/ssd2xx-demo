#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arm_neon.h>

#include "mi_common_datatype.h"
#include "mi_sys_datatype.h"
#include "mi_sys.h"
#include "mi_divp_datatype.h"
#include "mi_divp.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"

#define MAKE_YUYV_VALUE(y, u, v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0, 128, 128)
#define YUYV_WHITE MAKE_YUYV_VALUE(255, 128, 128)
#define YUYV_RED MAKE_YUYV_VALUE(76, 84, 255)
#define YUYV_GREEN MAKE_YUYV_VALUE(149, 43, 21)
#define YUYV_BLUE MAKE_YUYV_VALUE(29, 225, 107)

typedef struct
{
	int vw;
	int vh;
	MI_SYS_ChnPort_t stSrcChnPort;
	MI_SYS_ChnPort_t stDstChnPort;
	MI_SYS_BufConf_t stBufConf;
	MI_SYS_BufInfo_t stBufInfo;
	MI_SYS_BUF_HANDLE bufHandle;
} YUV_DISP_t;

void yuv_disp_destroy(void *handle);
void yuv_disp_show_i420(void *handle, const uint8_t **data);
void yuv_disp_show(void *handle, const uint8_t *buff);
void *yuv_disp_init(int vw, int vh, int ox, int oy, int ow, int oh);

void SigmaStar_DISP_Enable()
{
	MI_DISP_PubAttr_t stPubAttr = {0};
	stPubAttr.u32BgColor = YUYV_BLACK;
	stPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
	stPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
	MI_DISP_SetPubAttr(0, &stPubAttr);
	MI_DISP_Enable(0);

	void *deHandle = yuv_disp_init(176, 144, 0, 0, 320, 240);
	if (deHandle != NULL)
		yuv_disp_destroy(deHandle);
}

void *yuv_disp_init(int vw, int vh, int ox, int oy, int ow, int oh)
{
	YUV_DISP_t *mHandle = calloc(1, sizeof(YUV_DISP_t));
	if (mHandle == NULL)
	{
		printf("calloc failure, %s\n", strerror(errno));
		return NULL;
	}
	memset(mHandle, 0, sizeof(YUV_DISP_t));

	mHandle->vw = vw;
	mHandle->vh = vh;

	MI_DISP_VideoLayerAttr_t stLayerAttr = {0};
	stLayerAttr.stVidLayerDispWin.u16X = ox;
	stLayerAttr.stVidLayerDispWin.u16Y = oy;
	stLayerAttr.stVidLayerDispWin.u16Width = ow;
	stLayerAttr.stVidLayerDispWin.u16Height = oh;
	stLayerAttr.stVidLayerSize.u16Width = ow;
	stLayerAttr.stVidLayerSize.u16Height = oh;
	stLayerAttr.ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
	MI_DISP_BindVideoLayer(0, 0);
	MI_DISP_SetVideoLayerAttr(0, &stLayerAttr);
	MI_DISP_EnableVideoLayer(0);

	MI_DIVP_ChnAttr_t stAttr = {0};
	MI_DIVP_CreateChn(0, &stAttr);
	MI_DIVP_SetChnAttr(0, &stAttr);
	MI_DIVP_OutputPortAttr_t stOutputPortAttr = {0};
	stOutputPortAttr.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
	stOutputPortAttr.eCompMode = E_MI_SYS_COMPRESS_MODE_NONE;
	stOutputPortAttr.u32Width = ow;
	stOutputPortAttr.u32Height = oh;
	MI_DIVP_SetOutputPortAttr(0, &stOutputPortAttr);
	MI_DIVP_StartChn(0);

	//
	MI_DISP_InputPortAttr_t stInputPortAttr = {0};
	stInputPortAttr.stDispWin.u16X = ox;
	stInputPortAttr.stDispWin.u16Y = oy;
	stInputPortAttr.stDispWin.u16Width = ow;
	stInputPortAttr.stDispWin.u16Height = oh;
	stInputPortAttr.u16SrcWidth = ow;
	stInputPortAttr.u16SrcHeight = oh;
	MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
	MI_DISP_EnableInputPort(0, 0);
	MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

#if 0
	MI_SYS_ChnPort_t stSrcChnPort = {0};
	MI_SYS_ChnPort_t stDstChnPort = {0};
#endif
	mHandle->stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
	mHandle->stSrcChnPort.u32ChnId = 0;
	mHandle->stSrcChnPort.u32DevId = 0;
	mHandle->stSrcChnPort.u32PortId = 0;
	mHandle->stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
	mHandle->stDstChnPort.u32ChnId = 0;
	mHandle->stDstChnPort.u32DevId = 0;
	mHandle->stDstChnPort.u32PortId = 0;
	MI_SYS_BindChnPort(&mHandle->stSrcChnPort, &mHandle->stDstChnPort, 30, 30);

#if 0
	MI_SYS_BufConf_t stBufConf = {0};
	MI_SYS_BufInfo_t stBufInfo = {0};
	MI_SYS_BUF_HANDLE bufHandle = {0};
#endif
	mHandle->stBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
	mHandle->stBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
	mHandle->stBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
	mHandle->stBufConf.stFrameCfg.u16Width = vw;  // 视频宽度
	mHandle->stBufConf.stFrameCfg.u16Height = vh; // 视频高度
	mHandle->stBufConf.u32Flags = MI_SYS_MAP_VA;

	return mHandle;
}

void yuv_disp_show(void *handle, const uint8_t *buff)
{
	YUV_DISP_t *mHandle = (YUV_DISP_t *)handle;
	if (mHandle != NULL)
	{
		int w = mHandle->vw;
		int h = mHandle->vh;
		int s = w * h;
		// MI_SYS_GetCurPts(&stBufConf.u64TargetPts);
		if (MI_SYS_ChnInputPortGetBuf(&mHandle->stSrcChnPort, &mHandle->stBufConf, &mHandle->stBufInfo, &mHandle->bufHandle, 1000) == MI_SUCCESS)
		{
			memcpy(mHandle->stBufInfo.stFrameData.pVirAddr[0], buff, s);
			memcpy(mHandle->stBufInfo.stFrameData.pVirAddr[1], buff + s, s >> 1);
			if (MI_SYS_ChnInputPortPutBuf(mHandle->bufHandle, &mHandle->stBufInfo, FALSE) != MI_SUCCESS)
			{
				printf("MI_SYS_ChnInputPortPutBuf failure!\n");
			}
		}
		else
		{
			printf("MI_SYS_ChnInputPortGetBuf failure!\n");
		}
	}
}

void yuv_disp_show_i420(void *handle, const uint8_t **data)
{
	YUV_DISP_t *mHandle = (YUV_DISP_t *)handle;
	if (mHandle != NULL)
	{
		int w = mHandle->vw;
		int h = mHandle->vh;
		int s = w * h;
		int l = s >> 2;
		// MI_SYS_GetCurPts(&stBufConf.u64TargetPts);
		if (MI_SYS_ChnInputPortGetBuf(&mHandle->stSrcChnPort, &mHandle->stBufConf, &mHandle->stBufInfo, &mHandle->bufHandle, 1000) == MI_SUCCESS)
		{
			memcpy(mHandle->stBufInfo.stFrameData.pVirAddr[0], data[0], s);
			uint8_t *u = (uint8_t *)data[1];
			uint8_t *v = (uint8_t *)data[2];
			uint8_t *uv = (uint8_t *)mHandle->stBufInfo.stFrameData.pVirAddr[1];
#if 0
			for (int i = 0; i < l; i++) {
				*uv++ = *u++;
				*uv++ = *v++;
			}
#else
			uint8x16x2_t c;
			for (int i = 0; i < l; i += 16)
			{
				c.val[0] = vld1q_u8(u);
				c.val[1] = vld1q_u8(v);
				vst2q_u8(uv, c);
				u += 16;
				v += 16;
				uv += 32;
			}
#endif
			if (MI_SYS_ChnInputPortPutBuf(mHandle->bufHandle, &mHandle->stBufInfo, FALSE) != MI_SUCCESS)
			{
				printf("MI_SYS_ChnInputPortPutBuf failure!\n");
			}
		}
		else
		{
			printf("MI_SYS_ChnInputPortGetBuf failure!\n");
		}
	}
}

void yuv_disp_destroy(void *handle)
{
	YUV_DISP_t *mHandle = (YUV_DISP_t *)handle;
	if (mHandle != NULL)
	{
		MI_SYS_UnBindChnPort(&mHandle->stSrcChnPort, &mHandle->stDstChnPort);
		MI_DIVP_StopChn(0);
		MI_DIVP_DestroyChn(0);
		MI_DISP_DisableInputPort(0, 0);
		MI_DISP_DisableVideoLayer(0);
		MI_DISP_UnBindVideoLayer(0, 0);
		free(mHandle);
	}
}

static uint8_t yuv420sp[1280 * 720 * 3 / 2];

int main(int argc, char *argv[])
{
	if(argc < 4){
		printf("usage: %s yuv_filename dispinwidth dispinheight\r\n",argv[0]);
		return -1;
	}
	const char *filepath = argv[1];
	int vw = atoi(argv[2]);
	int vh = atoi(argv[3]);
	int ox = 0;
	int oy = 0;
	int ww = 800;
	int wh = 480;
	int fs = vw * vh * 3 / 2;
	int ys = vw * vh;
	int ws = ys >> 1;

	SigmaStar_DISP_Enable();

	FILE *fp = fopen(filepath, "r");
	if (fp != NULL)
	{
		void *handle = yuv_disp_init(vw, vh, ox, oy, ww, wh);
		while (1)
		{
			if (fread(yuv420sp, 1, fs, fp) != fs)
			{
				fseek(fp, 0, SEEK_SET);
				sleep(1);
				continue;
			}
			yuv_disp_show(handle, yuv420sp);
			usleep(30 * 1000);
		}

		fclose(fp);
	}

	return 0;
}
