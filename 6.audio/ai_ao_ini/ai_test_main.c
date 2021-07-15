/*********************************************************************************
  *FileName: ai_test_main.c
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-23
**********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_ai.h"
#include "mi_ao.h"
#include "audio_ai_par.h"


#define AI_INI_PATH "./audio_ai_par.ini"

#define WAV_G711A 	(0x06)
#define WAV_G711U 	(0x07)
#define WAV_G726 	(0x45)
#define WAV_PCM  	(0x1)

#define G726_16 	(2)
#define G726_24 	(3)
#define G726_32 	(4)
#define G726_40 	(5)

#define AI_AMIC_CHN_MAX 	        (2)
#define AI_DMIC_CHN_MAX		        (4)
#define AI_I2S_NOR_CHN 		        (2)
#define AI_LINE_IN_CHN_MAX 	        (2)
#define AI_AMIC_AND_I2S_RX_CHN_MAX 	(4)
#define AI_DMIC_AND_I2S_RX_CHN_MAX 	(4)

#define AI_DEV_ID_MAX   (6)
#define AO_DEV_ID_MAX   (2)

#define AI_DEV_AMIC             (0)
#define AI_DEV_DMIC             (1)
#define AI_DEV_I2S_RX           (2)
#define AI_DEV_LINE_IN          (3)
#define AI_DEV_AMIC_AND_I2S_RX  (4)
#define AI_DEV_DMIC_AND_I2S_RX  (5)

#define AO_DEV_LINE_OUT         (0)
#define AO_DEV_I2S_TX           (1)

#if defined(HDMI)
#define AO_DEV_HDMI   	(2)
#define AO_DEV_HDMI_AND_LINEOUT (3)
#endif

#define AI_VOLUME_AMIC_MIN      (0)
#define AI_VOLUME_AMIC_MAX      (21)
#define AI_VOLUME_LINEIN_MAX    (7)

#define AI_VOLUME_DMIC_MIN      (0)
#define AI_VOLUME_DMIC_MAX      (4)

#define AO_VOLUME_MIN           (-60)
#define AO_VOLUME_MAX           (30)

#define MI_AUDIO_SAMPLE_PER_FRAME	(1024)

#define DMA_BUF_SIZE_8K     (8000)
#define DMA_BUF_SIZE_16K    (16000)
#define DMA_BUF_SIZE_32K    (32000)
#define DMA_BUF_SIZE_48K    (48000)

#define AI_DMA_BUFFER_MAX_SIZE	(256 * 1024)
#define AI_DMA_BUFFER_MID_SIZE	(128 * 1024)
#define AI_DMA_BUFFER_MIN_SIZE	(64 * 1024)

#define AO_DMA_BUFFER_MAX_SIZE	(256 * 1024)
#define AO_DMA_BUFFER_MID_SIZE	(128 * 1024)
#define AO_DMA_BUFFER_MIN_SIZE	(64 * 1024)

#define MIU_WORD_BYTE_SIZE	(8)
#define TOTAL_BUF_DEPTH		(8)

typedef enum
{
	E_AI_SOUND_MODE_MONO = 0,
	E_AI_SOUND_MODE_STEREO,
	E_AI_SOUND_MODE_QUEUE,
} AiSoundMode_e;
typedef enum
{
    E_SOUND_MODE_MONO = 0, /* mono */
    E_SOUND_MODE_STEREO = 1, /* stereo */
} SoundMode_e;

typedef enum
{
    E_SAMPLE_RATE_8000 = 8000, /* 8kHz sampling rate */
    E_SAMPLE_RATE_16000 = 16000, /* 16kHz sampling rate */
    E_SAMPLE_RATE_32000 = 32000, /* 32kHz sampling rate */
    E_SAMPLE_RATE_48000 = 48000, /* 48kHz sampling rate */
} SampleRate_e;

typedef enum
{
    E_AENC_TYPE_G711A = 0,
    E_AENC_TYPE_G711U,
    E_AENC_TYPE_G726_16,
    E_AENC_TYPE_G726_24,
    E_AENC_TYPE_G726_32,
    E_AENC_TYPE_G726_40,
    PCM,
} AencType_e;

typedef struct WAVE_FORMAT
{
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    char chRIFF[4];
    unsigned int  dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int  dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int  dwDATALen;
} WaveFileHeader_t;

typedef struct AiOutFilenName_s
{
    MI_S8 *ps8OutputPath;
    MI_S8 *ps8OutputFile;
    MI_S8 *ps8SrcPcmOutputFile;
    MI_AUDIO_DEV AiDevId;
    MI_AUDIO_Attr_t stAiAttr;
    MI_BOOL bSetVqeWorkingSampleRate;
    MI_AUDIO_SampleRate_e eVqeWorkingSampleRate;
    MI_BOOL bEnableRes;
    MI_AUDIO_SampleRate_e eOutSampleRate;
    MI_BOOL bEnableAenc;
    MI_AI_AencConfig_t stSetAencConfig;
    MI_AI_VqeConfig_t stSetVqeConfig;
    MI_BOOL bEnableBf;
} AiOutFileName_t;

typedef struct AiChnPriv_s
{
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN AiChn;
    MI_S32 s32Fd;
    MI_U32 u32TotalSize;
    MI_U32 u32SrcPcmTotalSize;
    MI_U32 u32ChnCnt;
    MI_BOOL bEnableAed;
    pthread_t tid;
    MI_S32 s32SrcPcmFd;
} AiChnPriv_t;

static MI_BOOL  bEnableAI = TRUE;
static MI_U8*   pu8AiOutputPath = "./";
static MI_AUDIO_DEV AiDevId = 0;
static MI_U32   u32AiChnCnt = 1;
static MI_S32   s32AiVolume = 10;
static MI_BOOL  bAiSetVolume = TRUE;
static MI_AUDIO_SampleRate_e eAiSampleRate = E_MI_AUDIO_SAMPLE_RATE_8000;
static MI_AUDIO_SampleRate_e eAiOutputResampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;

static AiSoundMode_e eAiSoundMode = E_AI_SOUND_MODE_MONO;
static MI_BOOL  bAiEnableVqe = FALSE;
static MI_BOOL  bAiEnableHpf = FALSE;
static MI_BOOL  bAiEnableAgc = FALSE;
static MI_BOOL  bAiEnableNr = FALSE;
static MI_BOOL  bAiEnableEq = FALSE;
static MI_BOOL  bAiEnableAec = FALSE;
static MI_BOOL  bAiEnableResample = FALSE;
static MI_BOOL	bAiEnableSsl = FALSE;
static MI_BOOL	bAiEnableBf = FALSE;
static MI_BOOL  bAiEnableAenc = FALSE;
static MI_AUDIO_AencType_e eAiAencType = E_MI_AUDIO_AENC_TYPE_INVALID;
static MI_AUDIO_G726Mode_e eAiAencG726Mode = E_MI_AUDIO_G726_MODE_INVALID;
static SoundMode_e eAiWavSoundMode = E_SOUND_MODE_MONO;
static AencType_e eAiWavAencType = PCM;
static MI_BOOL  bAiEnableAed = FALSE;
static MI_AUDIO_SampleRate_e eAiWavSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_BOOL 	bAiI2sNormalMode = TRUE;
static MI_U32	u32MicDistance = 0;
static MI_U32 	u32VqeWorkingSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_BOOL  bAiDumpPcmData = FALSE;
static MI_BOOL  bAiSetBfDoa = FALSE;
static MI_S32   s32AiBfDoa = 0;
static MI_BOOL  bAiExit = FALSE;
static MI_U32   u32RunTime = 60;

static MI_S32   AiChnFd[MI_AUDIO_MAX_CHN_NUM] = {[0 ... MI_AUDIO_MAX_CHN_NUM-1] = -1};
static MI_S32   AiChnSrcPcmFd[MI_AUDIO_MAX_CHN_NUM] = {[0 ... MI_AUDIO_MAX_CHN_NUM-1] = -1};

static AiChnPriv_t stAiChnPriv[MI_AUDIO_MAX_CHN_NUM];
MI_AI_SslInitAttr_t stSslInit = {
	.bBfMode = FALSE,
};

MI_AI_SslConfigAttr_t stSslConfig = {
	.s32Temperature = 25,
	.s32NoiseGateDbfs = -40,
	.s32DirectionFrameNum = 300,
};

MI_AI_BfInitAttr_t stBfInit = {
	.u32ChanCnt = 2,
};

MI_AI_BfConfigAttr_t stBfConfig = {
	.s32Temperature = 25,
	.s32NoiseGateDbfs = -40,
	.s32NoiseSupressionMode = 8,
	.s32NoiseEstimation = 1,
	.outputGain = 0.7,
};

MI_AUDIO_HpfConfig_t stHpfCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .eHpfFreq = E_MI_AUDIO_HPF_FREQ_150,
};

MI_AI_AecConfig_t stAecCfg = {
    .bComfortNoiseEnable = FALSE,
    .s16DelaySample = 0,
    .u32AecSupfreq = {4, 6, 36, 49, 50, 51},
    .u32AecSupIntensity = {5, 4, 4, 5, 10, 10, 10},
};

MI_AUDIO_AnrConfig_t stAnrCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_MUSIC,
    .u32NrIntensity = 15,
    .u32NrSmoothLevel = 10,
    .eNrSpeed = E_MI_AUDIO_NR_SPEED_MID,
};

MI_AUDIO_AgcConfig_t stAgcCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .s32NoiseGateDb = -60,
    .s32TargetLevelDb = -3,
    .stAgcGainInfo = {
        .s32GainInit = 0,
        .s32GainMax = 20,
        .s32GainMin = 0,
    },
    .u32AttackTime = 1,
    .s16Compression_ratio_input = {-80, -60, -40, -25, 0},
    .s16Compression_ratio_output = {-80, -30, -15, -10, -3},
    .u32DropGainMax = 12,
    .u32NoiseGateAttenuationDb = 0,
    .u32ReleaseTime = 3,
};

MI_AUDIO_EqConfig_t stEqCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .s16EqGainDb = {[0 ... 128] = 3},
};

MI_AI_AedConfig_t stAedCfg = {
    .bEnableNr = TRUE,
    .eSensitivity = E_MI_AUDIO_AED_SEN_HIGH,
    .s32OperatingPoint = -5,
    .s32VadThresholdDb = -40,
    .s32LsdThresholdDb = -15,
};

MI_S32 initAi(void);
MI_S32 deinitAi(void);
void* aiGetChnPortBuf(void* data);
MI_S32 createOutputFileName(AiOutFileName_t *pstAiOutFileName, MI_U32 u32ChnIdx);
int addWaveHeader(WaveFileHeader_t* tWavHead, AencType_e eAencType, SoundMode_e eSoundMode, SampleRate_e eSampleRate, int raw_len);
void initParam(void);
void signalHandler(int signo);

int main(int argc, char *argv[])
{
    MI_U32      u32ChnIdx;
    MI_U32      u32ChnCnt;

    ExecFunc(MI_SYS_Init(), MI_SUCCESS);
    initParam();
    // enable ai
    if (bEnableAI)
    {
        ExecFunc(initAi(), MI_SUCCESS);
    }

    if ((E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    // join ai
    if (bEnableAI)
    {
        for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
            pthread_join(stAiChnPriv[u32ChnIdx].tid, NULL);
        }
        printf("join Ai thread done.\n");
    }

     // disable ai
    if (bEnableAI)
    {
        ExecFunc(deinitAi(), MI_SUCCESS);
    }

    printf("join Ai thread done.\n");

    ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
}


MI_S32 initAi(void)
{
    MI_AUDIO_Attr_t     stAiSetAttr;
    MI_AUDIO_Attr_t     stAiGetAttr;
    AiOutFileName_t     stAiFileName;
    MI_AI_AencConfig_t  stAiSetAencConfig, stAiGetAencConfig;
    MI_AI_VqeConfig_t   stAiSetVqeConfig, stAiGetVqeConfig;
    MI_AI_AedConfig_t   stAiSetAedConfig, stAiGetAedConfig;
    MI_U32              u32ChnIdx;
    MI_U32              u32ChnCnt;
    MI_S8               s8OutputFileName[512];
    MI_S8               s8SrcPcmOutputFileName[512];
    MI_S32              s32Ret;
    WaveFileHeader_t    stAiWavHead;
    MI_SYS_ChnPort_t    stAiChnOutputPort0[MI_AUDIO_MAX_CHN_NUM];
    MI_AI_ChnParam_t    stAiChnParam;
    MI_AI_SslInitAttr_t	stAiGetSslInitAttr;
    MI_AI_SslConfigAttr_t	stAiGetSslConfigAttr;
    MI_AI_BfInitAttr_t		stAiGetBfInitAttr;
    MI_AI_BfConfigAttr_t	stAiGetBfConfigAttr;

    memset(&stAiSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    memset(&stAiGetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAiSetAttr.eSamplerate = eAiSampleRate;
    stAiSetAttr.eSoundmode = eAiSoundMode;
    stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAiSetAttr.u32ChnCnt = u32AiChnCnt;
    stAiSetAttr.u32CodecChnCnt = 0; // useless
    stAiSetAttr.u32FrmNum = 6;  // useless
    stAiSetAttr.u32PtNumPerFrm = stAiSetAttr.eSamplerate / 16; // for aec
    stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE; // useless
    stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;

    if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
		|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
	{
		eAiWavSoundMode = E_SOUND_MODE_MONO;
	}
	else
	{
		eAiWavSoundMode = E_SOUND_MODE_STEREO;
	}

	if (bAiEnableBf)
	{
		eAiWavSoundMode = E_SOUND_MODE_MONO;
	}

    memset(&stAiSetVqeConfig, 0x0, sizeof(MI_AI_VqeConfig_t));
    if (bAiEnableVqe)
    {
		if ((E_AI_SOUND_MODE_MONO == eAiSoundMode)
			|| (E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
		{
			stAiSetVqeConfig.u32ChnNum = 1;
		}
		else
		{
			stAiSetVqeConfig.u32ChnNum = 2;
		}

		if (bAiEnableBf)
		{
			stAiSetVqeConfig.u32ChnNum = 1;
		}

        stAiSetVqeConfig.bAecOpen = bAiEnableAec;
        stAiSetVqeConfig.bAgcOpen = bAiEnableAgc;
        stAiSetVqeConfig.bAnrOpen = bAiEnableNr;
        stAiSetVqeConfig.bEqOpen = bAiEnableEq;
        stAiSetVqeConfig.bHpfOpen = bAiEnableHpf;
        stAiSetVqeConfig.s32FrameSample = 128;
        if ((E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
        {
        	stAiSetVqeConfig.s32WorkSampleRate = u32VqeWorkingSampleRate;
        }
        else
        {
			stAiSetVqeConfig.s32WorkSampleRate = eAiSampleRate;
        }

        // AEC
        memcpy(&stAiSetVqeConfig.stAecCfg, &stAecCfg, sizeof(MI_AI_AecConfig_t));

        // AGC
        memcpy(&stAiSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));

        // ANR
        memcpy(&stAiSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));

        // EQ
        memcpy(&stAiSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));

        // HPF
        memcpy(&stAiSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));
    }

    memset(&stAiSetAencConfig, 0x0, sizeof(MI_AI_AencConfig_t));
    if (bAiEnableAenc)
    {
        stAiSetAencConfig.eAencType = eAiAencType;
        if (E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType)
        {
            stAiSetAencConfig.stAencG726Cfg.eG726Mode = eAiAencG726Mode;
        }
    }

    memset(&stAiFileName, 0x0, sizeof(AiOutFileName_t));
    stAiFileName.AiDevId = AiDevId;
    stAiFileName.bEnableAenc = bAiEnableAenc;
    stAiFileName.bEnableRes = bAiEnableResample;
    stAiFileName.eOutSampleRate = eAiOutputResampleRate;
    stAiFileName.ps8OutputFile = (MI_S8*)s8OutputFileName;
    stAiFileName.ps8SrcPcmOutputFile = (MI_S8*)s8SrcPcmOutputFileName;
    stAiFileName.ps8OutputPath = (MI_S8*)pu8AiOutputPath;
    memcpy(&stAiFileName.stAiAttr, &stAiSetAttr, sizeof(MI_AUDIO_Attr_t));
    memcpy(&stAiFileName.stSetVqeConfig, &stAiSetVqeConfig, sizeof(MI_AI_VqeConfig_t));
    memcpy(&stAiFileName.stSetAencConfig, &stAiSetAencConfig, sizeof(MI_AI_AencConfig_t));

	if ((E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
    {
		stAiFileName.bSetVqeWorkingSampleRate = TRUE;
		stAiFileName.eVqeWorkingSampleRate = u32VqeWorkingSampleRate;
    }

    if (TRUE == bAiEnableBf)
    {
        stAiFileName.bEnableBf = TRUE;
    }

    memset(&stAiWavHead, 0x0, sizeof(WaveFileHeader_t));

    u32ChnCnt = u32AiChnCnt;

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        memset(s8OutputFileName, 0x0, sizeof(s8OutputFileName));
        memset(s8SrcPcmOutputFileName, 0x0, sizeof(s8SrcPcmOutputFileName));
        createOutputFileName(&stAiFileName, u32ChnIdx);
        printf("OutputName:%s\n", s8OutputFileName);
        AiChnFd[u32ChnIdx] = open((const char *)s8OutputFileName, O_RDWR | O_CREAT, 0777);
        if(AiChnFd[u32ChnIdx] < 0)
        {
            printf("Open output file path:%s fail \n", s8OutputFileName);
            printf("error:%s", strerror(errno));
            return -1;
        }

        s32Ret = write(AiChnFd[u32ChnIdx], &stAiWavHead, sizeof(WaveFileHeader_t));
        if (s32Ret < 0)
        {
            printf("write wav head failed\n");
            return -1;
        }

		if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
		{
            if (TRUE == bAiDumpPcmData)
            {
	            printf("OutputName:%s\n", s8SrcPcmOutputFileName);
	            AiChnSrcPcmFd[u32ChnIdx] = open((const char *)s8SrcPcmOutputFileName, O_RDWR | O_CREAT, 0777);
	            if(AiChnSrcPcmFd[u32ChnIdx] < 0)
	            {
	                printf("Open output file path:%s fail \n", s8SrcPcmOutputFileName);
	                printf("error:%s", strerror(errno));
	                return -1;
	            }

	            s32Ret = write(AiChnSrcPcmFd[u32ChnIdx], &stAiWavHead, sizeof(WaveFileHeader_t));
	            if (s32Ret < 0)
	            {
	                printf("write wav head failed\n");
	                return -1;
	            }
            }
        }
    }

    ExecFuncNoExit(MI_AI_SetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AI_GetPubAttr(AiDevId, &stAiGetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AI_Enable(AiDevId), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
		goto ERROR_RETURN;
    }

    if (bAiSetVolume)
    {
        for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
        {
            if ((AI_DEV_AMIC_AND_I2S_RX == AiDevId)
                || ((AI_DEV_DMIC_AND_I2S_RX == AiDevId)))
            {
                if ((E_MI_AUDIO_SOUND_MODE_MONO == eAiSoundMode)
                    || (E_MI_AUDIO_SOUND_MODE_QUEUE == eAiSoundMode))
                {
                    if ((2 == u32ChnIdx) || (3 == u32ChnIdx))
                    {
                        continue;
                    }
                }
                else if (E_MI_AUDIO_SOUND_MODE_STEREO == eAiSoundMode)
                {
                    if (1 == u32ChnIdx)
                    {
                        continue;
                    }
                }
            }
            ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, s32AiVolume), MI_SUCCESS);
        }
    }

#if 0
    // for test new API
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    stAiChnParam.stChnGain.bEnableGainSet = TRUE;
    stAiChnParam.stChnGain.s16FrontGain = 0;
    stAiChnParam.stChnGain.s16RearGain = 15;
    ExecFunc(MI_AI_SetChnParam(AiDevId, 0, &stAiChnParam), MI_SUCCESS);
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    ExecFunc(MI_AI_GetChnParam(AiDevId, 0, &stAiChnParam), MI_SUCCESS);
    printf("Dev%dChn%d bEnableGainSetting:%d Front Gain:%d, Rear Gain:%d.\n",
            AoDevId, 0, stAiChnParam.stChnGain.bEnableGainSet, stAiChnParam.stChnGain.s16FrontGain,
            stAiChnParam.stChnGain.s16RearGain);
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    stAiChnParam.stChnGain.bEnableGainSet = TRUE;
    stAiChnParam.stChnGain.s16FrontGain = 2;
    stAiChnParam.stChnGain.s16RearGain = 30;
    ExecFunc(MI_AI_SetChnParam(AiDevId, 1, &stAiChnParam), MI_SUCCESS);
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
    ExecFunc(MI_AI_GetChnParam(AiDevId, 1, &stAiChnParam), MI_SUCCESS);
    printf("Dev%dChn%d bEnableGainSetting:%d Front Gain:%d, Rear Gain:%d.\n",
            AoDevId, 1, stAiChnParam.stChnGain.bEnableGainSet, stAiChnParam.stChnGain.s16FrontGain,
            stAiChnParam.stChnGain.s16RearGain);
#endif
    memset(&stAiChnPriv, 0x0, sizeof(stAiChnPriv));
    memset(&stAiChnOutputPort0, 0x0, sizeof(stAiChnOutputPort0));

    if ((E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        stAiChnPriv[u32ChnIdx].AiChn = u32ChnIdx;
        stAiChnPriv[u32ChnIdx].AiDevId = AiDevId;
        stAiChnPriv[u32ChnIdx].s32Fd = AiChnFd[u32ChnIdx];
        stAiChnPriv[u32ChnIdx].s32SrcPcmFd = AiChnSrcPcmFd[u32ChnIdx];
        stAiChnPriv[u32ChnIdx].u32ChnCnt = u32AiChnCnt;
        stAiChnPriv[u32ChnIdx].u32TotalSize = 0;
        stAiChnPriv[u32ChnIdx].u32SrcPcmTotalSize = 0;

        stAiChnOutputPort0[u32ChnIdx].eModId = E_MI_MODULE_ID_AI;
        stAiChnOutputPort0[u32ChnIdx].u32DevId = AiDevId;
        stAiChnOutputPort0[u32ChnIdx].u32ChnId = u32ChnIdx;
        stAiChnOutputPort0[u32ChnIdx].u32PortId = 0;
        ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort0[u32ChnIdx], 1, TOTAL_BUF_DEPTH), MI_SUCCESS);
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        ExecFuncNoExit(MI_AI_EnableChn(AiDevId, u32ChnIdx), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
			goto DISABLE_DEVICE;
        }
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        audio_ai_par_execute(AiDevId, u32ChnIdx);
     
        // AED
        if (bAiEnableAed)
        {
        //     memcpy(&stAiSetAedConfig, &stAedCfg, sizeof(MI_AI_AedConfig_t));
        //     ExecFunc(MI_AI_SetAedAttr(AiDevId, u32ChnIdx, &stAiSetAedConfig), MI_SUCCESS);
        //     ExecFunc(MI_AI_GetAedAttr(AiDevId, u32ChnIdx, &stAiGetAedConfig), MI_SUCCESS);
        //     ExecFunc(MI_AI_EnableAed(AiDevId, u32ChnIdx), MI_SUCCESS);
            stAiChnPriv[u32ChnIdx].bEnableAed = TRUE;
        }

		// SSL
		if (bAiEnableSsl)
		{
			if ((TRUE == bAiEnableBf) && (FALSE == bAiSetBfDoa))
			{
				stSslInit.bBfMode = TRUE;
			}

			if (E_MI_AUDIO_SAMPLE_RATE_8000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 50;
			}
			else if (E_MI_AUDIO_SAMPLE_RATE_16000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 100;
			}
			else if (E_MI_AUDIO_SAMPLE_RATE_32000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 200;
			}
			else if (E_MI_AUDIO_SAMPLE_RATE_48000 == eAiSampleRate)
			{
				stSslConfig.s32DirectionFrameNum = 300;
			}

		// 	ExecFunc(MI_AI_SetSslInitAttr(AiDevId, u32ChnIdx, &stSslInit), MI_SUCCESS);
		// 	ExecFunc(MI_AI_GetSslInitAttr(AiDevId, u32ChnIdx, &stAiGetSslInitAttr), MI_SUCCESS);
		// 	ExecFunc(MI_AI_SetSslConfigAttr(AiDevId, u32ChnIdx, &stSslConfig), MI_SUCCESS);
		// 	ExecFunc(MI_AI_GetSslConfigAttr(AiDevId, u32ChnIdx, &stAiGetSslConfigAttr), MI_SUCCESS);
		// 	ExecFunc(MI_AI_EnableSsl(AiDevId, u32ChnIdx), MI_SUCCESS);
		}

		// BF
		// if (bAiEnableBf)
		// {
		// 	ExecFunc(MI_AI_SetBfInitAttr(AiDevId, u32ChnIdx, &stBfInit), MI_SUCCESS);
		// 	ExecFunc(MI_AI_GetBfInitAttr(AiDevId, u32ChnIdx, &stAiGetBfInitAttr), MI_SUCCESS);
		// 	ExecFunc(MI_AI_SetBfConfigAttr(AiDevId, u32ChnIdx, &stBfConfig), MI_SUCCESS);
		// 	ExecFunc(MI_AI_GetBfConfigAttr(AiDevId, u32ChnIdx, &stAiGetBfConfigAttr), MI_SUCCESS);
		// 	if (bAiSetBfDoa)
		// 	{
		// 	    ExecFunc(MI_AI_SetBfAngle(AiDevId, u32ChnIdx, s32AiBfDoa), MI_SUCCESS);
		// 	}
		// 	ExecFunc(MI_AI_EnableBf(AiDevId, u32ChnIdx), MI_SUCCESS);
		// }

        // VQE
        // if(bAiEnableVqe)
        // {
        //     ExecFunc(MI_AI_SetVqeAttr(AiDevId, u32ChnIdx, 0, 0, &stAiSetVqeConfig), MI_SUCCESS);
        //     ExecFunc(MI_AI_GetVqeAttr(AiDevId, u32ChnIdx, &stAiGetVqeConfig), MI_SUCCESS);
        //     ExecFunc(MI_AI_EnableVqe(AiDevId, u32ChnIdx), MI_SUCCESS);
        //     if ((E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
        //     {
        //     	eAiWavSampleRate = u32VqeWorkingSampleRate;
        //     }
        // }

        // RES
        // if(bAiEnableResample)
        // {
        //     ExecFunc(MI_AI_EnableReSmp(AiDevId, u32ChnIdx, eAiOutputResampleRate), MI_SUCCESS);
        //     eAiWavSampleRate = eAiOutputResampleRate;
        // }

        // AENC
        // if (bAiEnableAenc)
        // {
        //     ExecFunc(MI_AI_SetAencAttr(AiDevId, u32ChnIdx, &stAiSetAencConfig), MI_SUCCESS);
        //     ExecFunc(MI_AI_GetAencAttr(AiDevId, u32ChnIdx, &stAiGetAencConfig), MI_SUCCESS);
        //     ExecFunc(MI_AI_EnableAenc(AiDevId, u32ChnIdx), MI_SUCCESS);
        // }

        pthread_create(&stAiChnPriv[u32ChnIdx].tid, NULL, aiGetChnPortBuf, &stAiChnPriv[u32ChnIdx]);
    }
    printf("create ai thread.\n");
    return 0;

DISABLE_CHANNEL:
	for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
	{
		ExecFunc(MI_AI_DisableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
	}
DISABLE_DEVICE:
	ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);

ERROR_RETURN:
	return s32Ret;
}


MI_S32 deinitAi(void)
{
    MI_U32 u32ChnIdx;
    MI_U32 u32ChnCnt;

    if ((E_AI_SOUND_MODE_QUEUE == eAiSoundMode))
    {
        u32ChnCnt = 1;
    }
    else
    {
        u32ChnCnt = u32AiChnCnt;
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32ChnCnt; u32ChnIdx++)
    {
        audio_ai_par_close(AiDevId, u32ChnIdx);
        // if(bAiEnableAed)
        // {
        //     ExecFunc(MI_AI_DisableAed(AiDevId, u32ChnIdx), MI_SUCCESS);
        // }

        // if (bAiEnableAenc)
        // {
        //     ExecFunc(MI_AI_DisableAenc(AiDevId, u32ChnIdx), MI_SUCCESS);
        // }

        // if(bAiEnableResample)
        // {
        //     ExecFunc(MI_AI_DisableReSmp(AiDevId, u32ChnIdx), MI_SUCCESS);
        // }

        // if(bAiEnableVqe)
        // {
        //     ExecFunc(MI_AI_DisableVqe(AiDevId, u32ChnIdx), MI_SUCCESS);
        // }

		// if (bAiEnableSsl)
		// {
		// 	ExecFunc(MI_AI_DisableSsl(AiDevId, u32ChnIdx), MI_SUCCESS);
		// }

		// if (bAiEnableBf)
		// {
		// 	ExecFunc(MI_AI_DisableBf(AiDevId, u32ChnIdx), MI_SUCCESS);
		// }

        ExecFunc(MI_AI_DisableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
    }
    ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);
    return 0;
}


void* aiGetChnPortBuf(void* data)
{
    AiChnPriv_t* priv = (AiChnPriv_t*)data;
    MI_AUDIO_Frame_t stAiChFrame;
    MI_AUDIO_AecFrame_t stAecFrame;
    MI_S32 s32Ret;
    MI_U32 u32ChnIndex;
    struct timeval tv_before, tv_after;
    MI_S64 before_us, after_us;
	MI_AI_AedResult_t stAedResult;
    WaveFileHeader_t stWavHead;
    MI_S32 s32Doa;

    memset(&stAiChFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    if ((E_AI_SOUND_MODE_MONO == eAiSoundMode) || (E_AI_SOUND_MODE_STEREO == eAiSoundMode))
    {
        while(FALSE == bAiExit)
        {
            s32Ret = MI_AI_GetFrame(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, -1);
            if (MI_SUCCESS == s32Ret)
            {
                if (priv->bEnableAed)
                {
                    MI_AI_GetAedResult(priv->AiDevId, priv->AiChn, &stAedResult);
                    printf("Dev%d Chn%d, bAcousticEventDetected[%d] bLoudSoundDetected[%d].\n",
                        priv->AiDevId, priv->AiChn, stAedResult.bAcousticEventDetected, stAedResult.bLoudSoundDetected);
                }
				else if ((E_AI_SOUND_MODE_STEREO == eAiSoundMode) && (bAiEnableSsl) &&
				        ((!bAiEnableBf) || (bAiEnableBf && bAiSetBfDoa)))
                {
					MI_AI_GetSslDoa(AiDevId, priv->AiChn, &s32Doa);
					printf("SSL Doa is %d.\n", s32Doa);
                }
                else
                {
	                gettimeofday(&tv_before, NULL);
	                write(priv->s32Fd, stAiChFrame.apVirAddr[0], stAiChFrame.u32Len);
	                gettimeofday(&tv_after, NULL);
	                before_us = tv_before.tv_sec * 1000000 + tv_before.tv_usec;
	                after_us = tv_after.tv_sec * 1000000 + tv_after.tv_usec;
	                if (after_us - before_us > 10 * 1000)
	                {
	                    printf("Chn:%d, cost time:%lldus = %lldms.\n", priv->AiChn, after_us - before_us, (after_us - before_us) / 1000);
	                }
	                priv->u32TotalSize += stAiChFrame.u32Len;

	                if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
	                {
                        if (TRUE == bAiDumpPcmData)
                        {
	                	    write(priv->s32SrcPcmFd, stAiChFrame.apSrcPcmVirAddr[0], stAiChFrame.u32SrcPcmLen);
	                	    priv->u32SrcPcmTotalSize += stAiChFrame.u32SrcPcmLen;
	                    }
                    }
                }
                MI_AI_ReleaseFrame(priv->AiDevId, priv->AiChn,  &stAiChFrame,  NULL);
            }
            else
            {
                printf("Dev%dChn%d get frame failed!!!error:0x%x\n", priv->AiDevId, priv->AiChn, s32Ret);
            }
        }
        memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
        addWaveHeader(&stWavHead, eAiWavAencType, eAiWavSoundMode, eAiWavSampleRate, priv->u32TotalSize);
        lseek(priv->s32Fd, 0, SEEK_SET);
        write(priv->s32Fd, &stWavHead, sizeof(WaveFileHeader_t));
        close(priv->s32Fd);

        if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
        {
            if (TRUE == bAiDumpPcmData)
            {
			    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
                if (TRUE == bAiEnableBf)
                {
                    eAiWavSoundMode = E_SOUND_MODE_STEREO;
                }
	            addWaveHeader(&stWavHead, PCM, eAiWavSoundMode, eAiSampleRate, priv->u32SrcPcmTotalSize);
	            lseek(priv->s32SrcPcmFd, 0, SEEK_SET);
	            write(priv->s32SrcPcmFd, &stWavHead, sizeof(WaveFileHeader_t));
	            close(priv->s32SrcPcmFd);
            }
        }
    }
    else
    {
        while(FALSE == bAiExit)
        {
            s32Ret = MI_AI_GetFrame(priv->AiDevId, priv->AiChn, &stAiChFrame, &stAecFrame, -1);
            if(s32Ret == MI_SUCCESS)
            {
                for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
                {
                    if (priv->bEnableAed)
                    {
                        MI_AI_GetAedResult(priv->AiDevId, u32ChnIndex, &stAedResult);
                        printf("Dev%d Chn%d, bAcousticEventDetected[%d] bLoudSoundDetected[%d].\n",
                            priv->AiDevId, u32ChnIndex, stAedResult.bAcousticEventDetected, stAedResult.bLoudSoundDetected);
                    }
                    write(AiChnFd[u32ChnIndex], stAiChFrame.apVirAddr[u32ChnIndex], stAiChFrame.u32Len / priv->u32ChnCnt);

                    if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
	                {
                        if (TRUE == bAiDumpPcmData)
                        {
	                	    write(AiChnSrcPcmFd[u32ChnIndex], stAiChFrame.apSrcPcmVirAddr[u32ChnIndex],
	                		    stAiChFrame.u32SrcPcmLen / priv->u32ChnCnt);
                        }
                    }
                }
                priv->u32TotalSize += stAiChFrame.u32Len / priv->u32ChnCnt;

                if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
                {
                    if (TRUE == bAiDumpPcmData)
                    {
					    priv->u32SrcPcmTotalSize += stAiChFrame.u32SrcPcmLen / priv->u32ChnCnt;
                    }
                }
                MI_AI_ReleaseFrame(priv->AiDevId, priv->AiChn, &stAiChFrame,  NULL);
            }
        }
        memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
        addWaveHeader(&stWavHead, eAiWavAencType, eAiWavSoundMode, eAiWavSampleRate, priv->u32TotalSize);
        for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
        {
            lseek(AiChnFd[u32ChnIndex], 0, SEEK_SET);
            write(AiChnFd[u32ChnIndex], &stWavHead, sizeof(WaveFileHeader_t));
            close(AiChnFd[u32ChnIndex]);
        }

        if (bAiEnableBf || bAiEnableVqe || bAiEnableResample || bAiEnableAenc)
        {
            if (TRUE == bAiDumpPcmData)
            {
			    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
                if (TRUE == bAiEnableBf)
                {
                    eAiWavSoundMode = E_SOUND_MODE_STEREO;
                }
	            addWaveHeader(&stWavHead, PCM, eAiWavSoundMode, eAiSampleRate, priv->u32SrcPcmTotalSize);
	            for (u32ChnIndex = 0; u32ChnIndex < priv->u32ChnCnt; u32ChnIndex++)
	            {
	                lseek(AiChnSrcPcmFd[u32ChnIndex], 0, SEEK_SET);
	                write(AiChnSrcPcmFd[u32ChnIndex], &stWavHead, sizeof(WaveFileHeader_t));
	                close(AiChnSrcPcmFd[u32ChnIndex]);
	            }
            }
        }
    }

    return NULL;
}

int addWaveHeader(WaveFileHeader_t* tWavHead, AencType_e eAencType, SoundMode_e eSoundMode, SampleRate_e eSampleRate, int raw_len)
{
    tWavHead->chRIFF[0] = 'R';
    tWavHead->chRIFF[1] = 'I';
    tWavHead->chRIFF[2] = 'F';
    tWavHead->chRIFF[3] = 'F';

    tWavHead->chWAVE[0] = 'W';
    tWavHead->chWAVE[1] = 'A';
    tWavHead->chWAVE[2] = 'V';
    tWavHead->chWAVE[3] = 'E';

    tWavHead->chFMT[0] = 'f';
    tWavHead->chFMT[1] = 'm';
    tWavHead->chFMT[2] = 't';
    tWavHead->chFMT[3] = 0x20;
    tWavHead->dwFMTLen = 0x10;

    if(eAencType == E_AENC_TYPE_G711A)
    {
        tWavHead->wave.wFormatTag = 0x06;
    }

    if(eAencType == E_AENC_TYPE_G711U)
    {
        tWavHead->wave.wFormatTag = 0x07;
    }

    if(eAencType == E_AENC_TYPE_G711U || eAencType == E_AENC_TYPE_G711A)
    {
        if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wBitsPerSample = 8;//bitWidth;g711encode出来是8bit，这里需要写死
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.wChannels) / 8;
    }
    else if(eAencType == PCM)
    {
        if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x1;
        tWavHead->wave.wBitsPerSample = 16; //16bit
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = 1024;
    }
    else //g726
    {
		if(eSoundMode == E_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x45;
        switch(eAencType)
        {
            case E_AENC_TYPE_G726_40:
                tWavHead->wave.wBitsPerSample = 5;
                tWavHead->wave.wBlockAlign =  5;
                break;
            case E_AENC_TYPE_G726_32:
                tWavHead->wave.wBitsPerSample = 4;
                tWavHead->wave.wBlockAlign =  4;
                break;
            case E_AENC_TYPE_G726_24:
                tWavHead->wave.wBitsPerSample = 3;
                tWavHead->wave.wBlockAlign =  3;
                break;
            case E_AENC_TYPE_G726_16:
                tWavHead->wave.wBitsPerSample = 2;
                tWavHead->wave.wBlockAlign =  2;
                break;
            default:
                printf("eAencType error:%d\n", eAencType);
                return -1;
        }

        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
    }

    tWavHead->chDATA[0] = 'd';
    tWavHead->chDATA[1] = 'a';
    tWavHead->chDATA[2] = 't';
    tWavHead->chDATA[3] = 'a';
    tWavHead->dwDATALen = raw_len;
    tWavHead->dwRIFFLen = raw_len + sizeof(WaveFileHeader_t) - 8;

    return 0;
}

MI_S32 createOutputFileName(AiOutFileName_t *pstAiOutFileName, MI_U32 u32ChnIdx)
{
    char as8Tmp[512] = {0};
    char asAencG726Mode[4][15] = {"_G726_16", "_G726_24", "_G726_32", "_G726_40"};

    //set OutpuFile prefix
    memset(as8Tmp, 0, sizeof(as8Tmp));
    if ('/' == pstAiOutFileName->ps8OutputPath[strlen((char *)pstAiOutFileName->ps8OutputPath) - 1])
    {
        sprintf(as8Tmp, "%s", pstAiOutFileName->ps8OutputPath);
    }
    else
    {
        sprintf(as8Tmp, "%s/", pstAiOutFileName->ps8OutputPath);
    }
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set Chn num
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "Chn%d_", u32ChnIdx);
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile Device ID

    memset(as8Tmp, 0, sizeof(as8Tmp));
    if (AI_DEV_AMIC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Amic_");
    }
    else if (AI_DEV_DMIC == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "Dmic_");
    }
    else if (AI_DEV_I2S_RX == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "I2SRx_" );
    }
    else if (AI_DEV_LINE_IN == pstAiOutFileName->AiDevId)
    {
        sprintf(as8Tmp, "LineIn_" );
    }
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile SampleRate
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "%dK", pstAiOutFileName->stAiAttr.eSamplerate / 1000);
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile BitWidth
    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, "_%dbit", 16);
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    //set OutpuFile Channel Mode
    memset(as8Tmp, 0, sizeof(as8Tmp));
    if (E_MI_AUDIO_SOUND_MODE_MONO == pstAiOutFileName->stAiAttr.eSoundmode)
    {
        sprintf(as8Tmp, "_MONO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_STEREO == pstAiOutFileName->stAiAttr.eSoundmode)
    {
        sprintf(as8Tmp, "_STEREO");
    }
    else if (E_MI_AUDIO_SOUND_MODE_QUEUE == pstAiOutFileName->stAiAttr.eSoundmode)
    {
        sprintf(as8Tmp, "_QUEUE");
    }

    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);

    strcpy((char *)pstAiOutFileName->ps8SrcPcmOutputFile, (char *)pstAiOutFileName->ps8OutputFile);

	// set vqe working sample rate
	if (pstAiOutFileName->bSetVqeWorkingSampleRate)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_%dKVqeWork", pstAiOutFileName->eVqeWorkingSampleRate / 1000);
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    //set  OutpuFile ResampleRate
    if (pstAiOutFileName->bEnableRes)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_%dKRES", pstAiOutFileName->eOutSampleRate / 1000);
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    //************set OutpuFile Veq Mod START******************
	if (TRUE == pstAiOutFileName->stSetVqeConfig.bAecOpen)
	{
		memset(as8Tmp, 0, sizeof(as8Tmp));
		sprintf(as8Tmp, "_Aec");
		strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
	}

    if (TRUE == pstAiOutFileName->bEnableBf)
    {
		memset(as8Tmp, 0, sizeof(as8Tmp));
		sprintf(as8Tmp, "_Bf");
		strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if (TRUE == pstAiOutFileName->stSetVqeConfig.bHpfOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Hpf");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if (TRUE == pstAiOutFileName->stSetVqeConfig.bAnrOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Anr");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if (TRUE == pstAiOutFileName->stSetVqeConfig.bAgcOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Agc");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }

    if(TRUE == pstAiOutFileName->stSetVqeConfig.bEqOpen)
    {
        memset(as8Tmp, 0, sizeof(as8Tmp));
        sprintf(as8Tmp, "_Eq");
        strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    }
    //************set OutpuFile Veq Mod END******************

    // set OutpufFile Aenc
    if (TRUE == pstAiOutFileName->bEnableAenc)
    {
        if (E_MI_AUDIO_AENC_TYPE_G711A == pstAiOutFileName->stSetAencConfig.eAencType)
        {
            memset(as8Tmp, 0, sizeof(as8Tmp));
            sprintf(as8Tmp, "_G711a");
            strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
        }
        else if (E_MI_AUDIO_AENC_TYPE_G711U == pstAiOutFileName->stSetAencConfig.eAencType)
        {
            memset(as8Tmp, 0, sizeof(as8Tmp));
            sprintf(as8Tmp, "_G711u");
            strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
        }
        else if (E_MI_AUDIO_AENC_TYPE_G726 == pstAiOutFileName->stSetAencConfig.eAencType)
        {
            memset(as8Tmp, 0, sizeof(as8Tmp));
            sprintf(as8Tmp, asAencG726Mode[pstAiOutFileName->stSetAencConfig.stAencG726Cfg.eG726Mode]);
            strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
        }
    }

    memset(as8Tmp, 0, sizeof(as8Tmp));
    sprintf(as8Tmp, ".wav");
    strcat((char *)pstAiOutFileName->ps8OutputFile, as8Tmp);
    strcat((char *)pstAiOutFileName->ps8SrcPcmOutputFile, as8Tmp);
    return 0;
}

void signalHandler(int signo)
{
    switch (signo){
        case SIGALRM:
        case SIGINT:
            printf("Catch signal!!!\n");
            bAiExit = TRUE;
            break;
   }
   return;
}

void setTimer(MI_U32 u32RunTime)
{
    struct itimerval new_value, old_value;
    if (0 != u32RunTime)
    {
        new_value.it_value.tv_sec = u32RunTime;
        new_value.it_value.tv_usec = 0;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &new_value, &old_value);
    }
    return;
}
void initParam(void)
{
    /*加载解析AI参数,设置全局参数*/
    audio_ai_par_analysis(AiDevId, 0, AI_INI_PATH);
    audio_ai_par_info_t *par = audio_ai_get_par( AiDevId,  0);
    if(par){
      bAiEnableAed = par->bAiEnableAed;
      bAiEnableSsl = par->bAiEnableSsl;
      bAiEnableBf = par->bAiEnableBf;
      bAiSetBfDoa = par->bAiSetBfDoa;
      s32AiBfDoa = par->s32AiBfDoa;
      bAiEnableVqe = par->bAiEnableVqe;
      bAiEnableResample = par->bAiEnableResample;
      bAiEnableAenc = par->bAiEnableAenc;
      eAiSampleRate = par->stAiSetVqeConfig.s32WorkSampleRate;
        // memcpy(&stAiSetAedConfig,&par->stAiSetAedConfig,sizeof(stAiSetAedConfig));
        memcpy(&stSslInit,&par->stSslInit,sizeof(stSslInit));
        memcpy(&stSslConfig,&par->stSslConfig,sizeof(stSslConfig));
        memcpy(&stBfInit,&par->stBfInit,sizeof(stBfInit));
        memcpy(&stBfConfig,&par->stBfConfig,sizeof(stBfConfig));
        memcpy(&eAiOutputResampleRate,&par->eAiOutputResampleRate,sizeof(eAiOutputResampleRate));
        // memcpy(&stAiSetAencConfig,&par->stAiSetAencConfig,sizeof(stAiSetAencConfig));
        // memcpy(&stAiSetAedConfig,&par->stAiSetAedConfig,sizeof(stAiSetAedConfig));
    }

    eAiWavSampleRate = eAiSampleRate;

    if (E_MI_AUDIO_AENC_TYPE_G711A == eAiAencType)
    {
        eAiWavAencType = E_AENC_TYPE_G711A;
    }
    else if (E_MI_AUDIO_AENC_TYPE_G711U == eAiAencType)
    {
        eAiWavAencType = E_AENC_TYPE_G711U;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_16 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_16;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_24 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_24;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_32 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_32;
    }
    else if ((E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType) && (E_MI_AUDIO_G726_MODE_40 == eAiAencG726Mode))
    {
        eAiWavAencType = E_AENC_TYPE_G726_40;
    }
    else
    {
        eAiWavAencType = PCM;
    }

	if (AI_DEV_I2S_RX == AiDevId)
	{
		if (
			(((E_AI_SOUND_MODE_MONO == eAiSoundMode) || (E_AI_SOUND_MODE_QUEUE == eAiSoundMode)) && (AI_I2S_NOR_CHN == u32AiChnCnt))
			|| ((E_AI_SOUND_MODE_STEREO == eAiSoundMode) && (AI_I2S_NOR_CHN / 2 == u32AiChnCnt)))
		{
			bAiI2sNormalMode = TRUE;
		}
		else
		{
			bAiI2sNormalMode = FALSE;
		}
	}

	if (bAiEnableSsl || bAiEnableBf)
	{
		stSslInit.u32MicDistance = u32MicDistance;
		stBfInit.u32MicDistance = u32MicDistance;
	}

    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);

    setTimer(u32RunTime);
    return;
}
