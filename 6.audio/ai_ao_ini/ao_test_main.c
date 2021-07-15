/*********************************************************************************
  *FileName: ao_test_main.c
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
#include "mi_ao.h"
#include "audio_ao_par.h"

#define AO_INI_PATH "./audio_ao_par.ini"
/*AO 测试 bEnableAO = TRUE*/
static MI_U8 *pu8AoInputPath = (MI_U8 *)"./8K_16bit_MONO_60s.wav";
static MI_BOOL bEnableAO = TRUE;
static MI_AUDIO_DEV AoDevId = 0;
static MI_AO_CHN AoChn = 0;

#define WAV_G711A (0x06)
#define WAV_G711U (0x07)
#define WAV_G726 (0x45)
#define WAV_PCM (0x1)

#define G726_16 (2)
#define G726_24 (3)
#define G726_32 (4)
#define G726_40 (5)

#define AI_AMIC_CHN_MAX (2)
#define AI_DMIC_CHN_MAX (4)
#define AI_I2S_NOR_CHN (2)
#define AI_LINE_IN_CHN_MAX (2)
#define AI_AMIC_AND_I2S_RX_CHN_MAX (4)
#define AI_DMIC_AND_I2S_RX_CHN_MAX (4)

#define AI_DEV_ID_MAX (6)
#define AO_DEV_ID_MAX (2)

#define AI_DEV_AMIC (0)
#define AI_DEV_DMIC (1)
#define AI_DEV_I2S_RX (2)
#define AI_DEV_LINE_IN (3)
#define AI_DEV_AMIC_AND_I2S_RX (4)
#define AI_DEV_DMIC_AND_I2S_RX (5)

#define AO_DEV_LINE_OUT (0)
#define AO_DEV_I2S_TX (1)

#define AI_VOLUME_AMIC_MIN (0)
#define AI_VOLUME_AMIC_MAX (21)
#define AI_VOLUME_LINEIN_MAX (7)

#define AI_VOLUME_DMIC_MIN (0)
#define AI_VOLUME_DMIC_MAX (4)

#define AO_VOLUME_MIN (-60)
#define AO_VOLUME_MAX (30)

#define MI_AUDIO_SAMPLE_PER_FRAME (1024)

#define DMA_BUF_SIZE_8K (8000)
#define DMA_BUF_SIZE_16K (16000)
#define DMA_BUF_SIZE_32K (32000)
#define DMA_BUF_SIZE_48K (48000)

#define AI_DMA_BUFFER_MAX_SIZE (256 * 1024)
#define AI_DMA_BUFFER_MID_SIZE (128 * 1024)
#define AI_DMA_BUFFER_MIN_SIZE (64 * 1024)

#define AO_DMA_BUFFER_MAX_SIZE (256 * 1024)
#define AO_DMA_BUFFER_MID_SIZE (128 * 1024)
#define AO_DMA_BUFFER_MIN_SIZE (64 * 1024)

#define MIU_WORD_BYTE_SIZE (8)
#define TOTAL_BUF_DEPTH (8)

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
    unsigned int dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int dwDATALen;
} WaveFileHeader_t;



static MI_BOOL bAoExit = FALSE;
MI_U8 u8TempBuf[MI_AUDIO_SAMPLE_PER_FRAME * 4];
static MI_AUDIO_SampleRate_e eAoInSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_AUDIO_SampleRate_e eAoOutSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
static MI_BOOL bAoEnableResample = FALSE;
static MI_BOOL bAoEnableAdec = FALSE;

static MI_S32 AoReadFd = -1;
static WaveFileHeader_t stWavHeaderInput;
static MI_S32 s32NeedSize = 0;
static pthread_t Aotid;

MI_S32 initAo(void);
MI_S32 deinitAo(void);
void *aoSendFrame(void *data);

int main(int argc, char *argv[])
{
    if(argc >= 2){
        pu8AoInputPath = argv[1];
    }

    audio_ao_par_analysis(AoDevId, AoChn, AO_INI_PATH);

    ExecFunc(MI_SYS_Init(), MI_SUCCESS);

    if (bEnableAO)
    {
        ExecFunc(initAo(), MI_SUCCESS);
    }

    // join ao
    if (bEnableAO)
    {
        pthread_join(Aotid, NULL);
        printf("join Ao thread done.\n");
    }

    // disable ao
    if (bEnableAO)
    {
        ExecFunc(deinitAo(), MI_SUCCESS);
    }

    ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
}

MI_S32 initAo(void)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Attr_t stAoSetAttr, stAoGetAttr;
    MI_AO_AdecConfig_t stAoSetAdecConfig, stAoGetAdecConfig;
    MI_S32 s32AoGetVolume;
    MI_AO_ChnParam_t stAoChnParam;
    MI_U32 u32DmaBufSize;
    MI_BOOL bInitAdec = FALSE, bInitRes = FALSE;

    AoReadFd = open((const char *)pu8AoInputPath, O_RDONLY, 0666);
    if (AoReadFd <= 0)
    {
        printf("Open input file failed:%s \n", pu8AoInputPath);
        printf("error:%s", strerror(errno));
        return -1;
    }

    s32Ret = read(AoReadFd, &stWavHeaderInput, sizeof(WaveFileHeader_t));
    if (s32Ret < 0)
    {
        printf("Read wav header failed!!!\n");
        return -1;
    }

    memset(&stAoSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    stAoSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAoSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAoSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    stAoSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
    stAoSetAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME;
    stAoSetAttr.u32ChnCnt = stWavHeaderInput.wave.wChannels;

    if (stAoSetAttr.u32ChnCnt == 2)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if (stAoSetAttr.u32ChnCnt == 1)
    {
        stAoSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    stAoSetAttr.eSamplerate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
    eAoInSampleRate = (MI_AUDIO_SampleRate_e)stWavHeaderInput.wave.dwSamplesPerSec;
    if (bAoEnableResample)
    {
        stAoSetAttr.eSamplerate = eAoOutSampleRate;
    }

    memset(&stAoSetAdecConfig, 0x0, sizeof(MI_AO_AdecConfig_t));
    if (WAV_G711A == stWavHeaderInput.wave.wFormatTag)
    {
        bAoEnableAdec = TRUE;
        stAoSetAdecConfig.eAdecType = E_MI_AUDIO_ADEC_TYPE_G711A;
        stAoSetAdecConfig.stAdecG711Cfg.eSamplerate = stAoSetAttr.eSamplerate;
        stAoSetAdecConfig.stAdecG711Cfg.eSoundmode = stAoSetAttr.eSoundmode;
    }
    else if (WAV_G711U == stWavHeaderInput.wave.wFormatTag)
    {
        bAoEnableAdec = TRUE;
        stAoSetAdecConfig.eAdecType = E_MI_AUDIO_ADEC_TYPE_G711U;
        stAoSetAdecConfig.stAdecG711Cfg.eSamplerate = stAoSetAttr.eSamplerate;
        stAoSetAdecConfig.stAdecG711Cfg.eSoundmode = stAoSetAttr.eSoundmode;
    }
    else if (WAV_G726 == stWavHeaderInput.wave.wFormatTag)
    {
        bAoEnableAdec = TRUE;
        stAoSetAdecConfig.eAdecType = E_MI_AUDIO_ADEC_TYPE_G726;
        stAoSetAdecConfig.stAdecG726Cfg.eSamplerate = stAoSetAttr.eSamplerate;
        stAoSetAdecConfig.stAdecG726Cfg.eSoundmode = stAoSetAttr.eSoundmode;

        switch (stWavHeaderInput.wave.wBitsPerSample)
        {
        case G726_16:
            stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_16;
            break;
        case G726_24:
            stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_24;
            break;
        case G726_32:
            stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_32;
            break;
        case G726_40:
            stAoSetAdecConfig.stAdecG726Cfg.eG726Mode = E_MI_AUDIO_G726_MODE_40;
            break;
        default:
            printf("G726 Mode Error:%d\n", stWavHeaderInput.wave.wBitsPerSample);
            return -1;
        }
    }

    ExecFuncNoExit(MI_AO_SetPubAttr(AoDevId, &stAoSetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
        goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AO_GetPubAttr(AoDevId, &stAoGetAttr), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
        goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AO_Enable(AoDevId), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
        goto ERROR_RETURN;
    }

    ExecFuncNoExit(MI_AO_EnableChn(AoDevId, AoChn), MI_SUCCESS, s32Ret);
    if (MI_SUCCESS != s32Ret)
    {
        goto DISABLE_DEVICE;
    }

    if (bAoEnableAdec)
    {
        ExecFuncNoExit(MI_AO_SetAdecAttr(AoDevId, AoChn, &stAoSetAdecConfig), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
            goto DISABLE_ALG;
        }

        ExecFuncNoExit(MI_AO_GetAdecAttr(AoDevId, AoChn, &stAoGetAdecConfig), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
            goto DISABLE_ALG;
        }

        ExecFuncNoExit(MI_AO_EnableAdec(AoDevId, AoChn), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
            goto DISABLE_ALG;
        }

        bInitAdec = TRUE;
    }

    if (bAoEnableResample)
    {
        ExecFuncNoExit(MI_AO_EnableReSmp(AoDevId, AoChn, eAoInSampleRate), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
            goto DISABLE_ALG;
        }

        bInitRes = TRUE;
    }

    /*从配置文件中加载ao属性*/
    audio_ao_par_execute(AoDevId, AoChn);

    s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 2 * (stAoSetAttr.u32ChnCnt);
    if (E_MI_AUDIO_SAMPLE_RATE_8000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_8K;
        ;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_16000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_16K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_32000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_32K;
    }
    else if (E_MI_AUDIO_SAMPLE_RATE_48000 == stAoSetAttr.eSamplerate)
    {
        u32DmaBufSize = DMA_BUF_SIZE_48K;
    }

    if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_STEREO)
    {
        if (s32NeedSize > (u32DmaBufSize / 4))
        {
            s32NeedSize = u32DmaBufSize / 4;
        }
    }
    else if (stAoSetAttr.eSoundmode == E_MI_AUDIO_SOUND_MODE_MONO)
    {
        if (s32NeedSize > (u32DmaBufSize / 8))
        {
            s32NeedSize = u32DmaBufSize / 8;
        }
    }

    if (bAoEnableResample)
    {
        s32NeedSize = s32NeedSize * eAoInSampleRate / eAoOutSampleRate;
        if (s32NeedSize > MI_AUDIO_SAMPLE_PER_FRAME * 4)
        {
            s32NeedSize = MI_AUDIO_SAMPLE_PER_FRAME * 4;
        }
    }

    if (bAoEnableAdec)
    {
        if (stAoSetAdecConfig.eAdecType == E_MI_AUDIO_ADEC_TYPE_G711A || stAoSetAdecConfig.eAdecType == E_MI_AUDIO_ADEC_TYPE_G711U)
        {
            s32NeedSize /= 2;
        }
        else if (stAoSetAdecConfig.eAdecType == E_MI_AUDIO_ADEC_TYPE_G726)
        {
            switch (stAoSetAdecConfig.stAdecG726Cfg.eG726Mode)
            {
            case E_MI_AUDIO_G726_MODE_16:
                s32NeedSize = s32NeedSize * 2 / 16;
                break;
            case E_MI_AUDIO_G726_MODE_24:
                s32NeedSize = s32NeedSize * 3 / 16;
                break;
            case E_MI_AUDIO_G726_MODE_32:
                s32NeedSize = s32NeedSize * 4 / 16;
                break;
            case E_MI_AUDIO_G726_MODE_40:
                s32NeedSize = s32NeedSize * 5 / 16;
                break;
            default:
                assert(0);
                break;
            }
        }
    }

    s32NeedSize = s32NeedSize / (stAoSetAttr.u32ChnCnt * 2 * 1) * (stAoSetAttr.u32ChnCnt * 2 * 1);
    pthread_create(&Aotid, NULL, aoSendFrame, NULL);
    printf("create ao thread.\n");
    return 0;

DISABLE_ALG:
    if (bAoEnableAdec && bInitAdec)
    {
        ExecFunc(MI_AO_DisableAdec(AoDevId, AoChn), MI_SUCCESS);
    }
    if (bAoEnableResample && bInitRes)
    {
        ExecFunc(MI_AO_DisableReSmp(AoDevId, AoChn), MI_SUCCESS);
    }

DISABLE_CHANNEL:
    ExecFunc(MI_AO_DisableChn(AoDevId, AoChn), MI_SUCCESS);
DISABLE_DEVICE:
    ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);
ERROR_RETURN:
    return s32Ret;
}

MI_S32 deinitAo(void)
{
    if (bAoEnableAdec)
    {
        ExecFunc(MI_AO_DisableAdec(AoDevId, AoChn), MI_SUCCESS);
    }

    if (bAoEnableResample)
    {
        ExecFunc(MI_AO_DisableReSmp(AoDevId, AoChn), MI_SUCCESS);
    }

    audio_ao_par_close(AoDevId, AoChn);

    ExecFunc(MI_AO_DisableChn(AoDevId, AoChn), MI_SUCCESS);
    ExecFunc(MI_AO_Disable(AoDevId), MI_SUCCESS);

    return 0;
}
void *aoSendFrame(void *data)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_Frame_t stAoSendFrame;

    while (FALSE == bAoExit)
    {
        s32Ret = read(AoReadFd, &u8TempBuf, s32NeedSize);
        if (s32Ret != s32NeedSize)
        {
            lseek(AoReadFd, sizeof(WaveFileHeader_t), SEEK_SET);
            s32Ret = read(AoReadFd, &u8TempBuf, s32NeedSize);
            if (s32Ret < 0)
            {
                printf("Input file does not has enough data!!!\n");
                break;
            }
        }

        memset(&stAoSendFrame, 0x0, sizeof(MI_AUDIO_Frame_t));
        stAoSendFrame.u32Len = s32Ret;
        stAoSendFrame.apVirAddr[0] = u8TempBuf;
        stAoSendFrame.apVirAddr[1] = NULL;

        do
        {
            s32Ret = MI_AO_SendFrame(AoDevId, AoChn, &stAoSendFrame, -1);
        } while (s32Ret == MI_AO_ERR_NOBUF);

        if (s32Ret != MI_SUCCESS)
        {
            printf("[Warning]: MI_AO_SendFrame fail, error is 0x%x: \n", s32Ret);
        }
    }
    close(AoReadFd);
    return NULL;
}
