/*********************************************************************************
  *FileName:  audio_ai_par.h
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-25
**********************************************************************************/
#ifndef _AUDIO_AI_PAR_H_
#define _AUDIO_AI_PAR_H_
#include "audio_par_common.h"
#include "audio_ini_default.h"
#include "iniparser.h"
#include "mi_ai.h"

/*参数见API文档说明*/
typedef struct audio_ai_par_info_t_
{
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN AiChn;

    MI_BOOL bAiEnableAed;
    MI_AI_AedConfig_t stAiSetAedConfig;

    MI_BOOL bAiEnableSsl;
    MI_AI_SslInitAttr_t stSslInit;
    MI_AI_SslConfigAttr_t stSslConfig;

    MI_BOOL bAiEnableBf;
    MI_AI_BfInitAttr_t stBfInit;
    MI_AI_BfConfigAttr_t stBfConfig;

    MI_BOOL bAiSetBfDoa;
    MI_S32 s32AiBfDoa;

    MI_BOOL bAiEnableVqe;
    MI_AUDIO_DEV AoDevId;
    MI_AO_CHN AoChn;
    MI_AI_VqeConfig_t stAiSetVqeConfig;

    MI_BOOL bAiEnableResample;
    MI_AUDIO_SampleRate_e eAiOutputResampleRate;

    MI_BOOL bAiEnableAenc;
    MI_AI_AencConfig_t stAiSetAencConfig;
} audio_ai_par_info_t;

/*-------------------------------------------------------------------------*/
/*
    接口功能同audio_ao_par.h类似
*/
/*--------------------------------------------------------------------------*/
int audio_ai_par_analysis(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn, const char *ini_path);
int audio_ai_par_execute(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn);
audio_ai_par_info_t *audio_ai_get_par(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn);
int audio_ai_par_close(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn);

#endif
