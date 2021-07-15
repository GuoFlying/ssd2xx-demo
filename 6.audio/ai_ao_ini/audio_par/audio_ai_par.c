/*********************************************************************************
  *FileName:  audio_ai_par.c
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-25
**********************************************************************************/
#include "audio_ai_par.h"
#include <stdlib.h>
#include <assert.h>

#define EN_INI_PARSER 1
#if EN_INI_PARSER
static MI_BOOL bAoEnableVqe = FALSE;
#define DEF_CMP_STR(def) \
    struct               \
    {                    \
        char *str;       \
        def e;           \
    }

static int create_default_ini_file(const char *ini_name, const char *def);

static int audio_ai_par_load_ini(const char *ini_path, audio_ai_par_info_t *info);

static int audio_ai_aed_load_ini(dictionary *ini, MI_AI_AedConfig_t *cfg);
static int audio_ai_ssl_load_ini(dictionary *ini, MI_AI_SslInitAttr_t *attr, MI_AI_SslConfigAttr_t *cfg);
static int audio_ai_bf_load_ini(dictionary *ini, MI_AI_BfInitAttr_t *attr, MI_AI_BfConfigAttr_t *cfg);
static int audio_ai_rate_load_ini(dictionary *ini, MI_AUDIO_SampleRate_e *ae);
static int audio_ai_aec_load_ini(dictionary *ini, MI_AI_AecConfig_t *cfg);
static int audio_ai_vqe_load_ini(dictionary *ini, MI_AI_VqeConfig_t *cfg, MI_AUDIO_DEV *AoDevId, MI_AO_CHN *AoChn);
static int audio_ai_aenc_load_ini(dictionary *ini, MI_AI_AencConfig_t *cfg);

static int audio_ai_aec_load_ini(dictionary *ini, MI_AI_AecConfig_t *cfg);
static int audio_ai_agc_load_ini(dictionary *ini, MI_AUDIO_AgcConfig_t *cfg);
static int audio_ai_anr_load_ini(dictionary *ini, MI_AUDIO_AnrConfig_t *cfg);
static int audio_ai_eq_load_ini(dictionary *ini, MI_AUDIO_EqConfig_t *cfg);
static int audio_ai_hpf_load_ini(dictionary *ini, MI_AUDIO_HpfConfig_t *cfg);

#else

static MI_AUDIO_HpfConfig_t stHpfCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .eHpfFreq = E_MI_AUDIO_HPF_FREQ_150,
};

static MI_AUDIO_AnrConfig_t stAnrCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_MUSIC,
    .u32NrIntensity = 15,
    .u32NrSmoothLevel = 10,
    .eNrSpeed = E_MI_AUDIO_NR_SPEED_MID,
};

static MI_AUDIO_AgcConfig_t stAgcCfg = {
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

static MI_AUDIO_EqConfig_t stEqCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .s16EqGainDb = {[0 ... 128] = 3},
};

static MI_AI_AedConfig_t stAedCfg = {
    .bEnableNr = TRUE,
    .eSensitivity = E_MI_AUDIO_AED_SEN_HIGH,
    .s32OperatingPoint = -5,
    .s32VadThresholdDb = -40,
    .s32LsdThresholdDb = -15,
};

static MI_AI_SslInitAttr_t stSslInit = {
    .bBfMode = FALSE,
};

static MI_AI_SslConfigAttr_t stSslConfig = {
    .s32Temperature = 25,
    .s32NoiseGateDbfs = -40,
    .s32DirectionFrameNum = 300,
};

static MI_AI_BfInitAttr_t stBfInit = {
    .u32ChanCnt = 2,
};

static MI_AI_BfConfigAttr_t stBfConfig = {
    .s32Temperature = 25,
    .s32NoiseGateDbfs = -40,
    .s32NoiseSupressionMode = 8,
    .s32NoiseEstimation = 1,
    .outputGain = 0.7,
};
static MI_AI_AecConfig_t stAecCfg = {
    .bComfortNoiseEnable = FALSE,
    .s16DelaySample = 0,
    .u32AecSupfreq = {4, 6, 36, 49, 50, 51},
    .u32AecSupIntensity = {5, 4, 4, 5, 10, 10, 10},
};
static MI_AI_VqeConfig_t stAiSetVqeConfig;
static MI_AI_AencConfig_t stAiSetAencConfig;

static MI_S32 s32AiBfDoa = 0;

static MI_BOOL bAiEnableAed = FALSE;
static MI_BOOL bAiEnableSsl = FALSE;
static MI_BOOL bAiEnableBf = FALSE;
static MI_BOOL bAiSetBfDoa = FALSE;
static MI_BOOL bAiEnableAenc = FALSE;
static MI_BOOL bAiEnableResample = FALSE;

static MI_BOOL bAiEnableVqe = TRUE;
static MI_BOOL bAiEnableHpf = TRUE;
static MI_BOOL bAiEnableAgc = TRUE;
static MI_BOOL bAiEnableNr = TRUE;
static MI_BOOL bAiEnableEq = TRUE;
static MI_BOOL bAiEnableAec = TRUE;

static MI_AUDIO_DEV AoDevId = 0;
static MI_AO_CHN AoChn = 0;
static MI_AUDIO_SampleRate_e eAiSampleRate = E_MI_AUDIO_SAMPLE_RATE_8000;
static MI_AUDIO_AencType_e eAiAencType = E_MI_AUDIO_AENC_TYPE_INVALID;
static MI_AUDIO_SampleRate_e eAiOutputResampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;

#endif

typedef struct ai_list_node_t_
{
    MI_AUDIO_DEV AiDevId;
    MI_AI_CHN AiChn;

    audio_ai_par_info_t par;

    struct ai_list_node_t_ *next;
} ai_list_node_t;

typedef struct ai_info_t_
{
    ai_list_node_t *begin;
    ai_list_node_t *end;
} ai_info_t;

static ai_info_t ai_info = {
    .begin = NULL,
    .end = NULL};

static ai_list_node_t *create_node(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
static ai_list_node_t *get_node(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
static int delete_node(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
static int audio_ai_par_set(audio_ai_par_info_t *info);

int audio_ai_par_analysis(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, const char *ini_path)
{
    ai_list_node_t *node = get_node(AiDevId, AiChn);
    if (!node)
    {
        node = create_node(AiDevId, AiChn);
    }

    if (!node)
    {
        return -1;
    }

    node->par.AiDevId = AiDevId;
    node->par.AiChn = AiChn;

#if EN_INI_PARSER
    if (0 == audio_ai_par_load_ini(ini_path, &node->par))
    {
        return 0;
    }

#else

    node->par.bAiEnableAed = bAiEnableAed;
    memcpy(&node->par.stAiSetAedConfig, &stAedCfg, sizeof(stAedCfg));

    node->par.bAiEnableSsl = bAiEnableSsl;
    memcpy(&node->par.stSslInit, &stSslInit, sizeof(stSslInit));
    memcpy(&node->par.stSslConfig, &stSslConfig, sizeof(stSslConfig));

    node->par.bAiEnableBf = bAiEnableBf;
    memcpy(&node->par.stBfInit, &stBfInit, sizeof(stBfInit));
    memcpy(&node->par.stBfConfig, &stBfConfig, sizeof(stBfConfig));

    node->par.bAiSetBfDoa = bAiSetBfDoa;
    node->par.s32AiBfDoa = s32AiBfDoa;

    node->par.bAiEnableVqe = bAiEnableVqe;
    node->par.AoDevId = AoDevId;
    node->par.AoChn = AoChn;
    memset(&stAiSetVqeConfig, 0x0, sizeof(MI_AI_VqeConfig_t));
    stAiSetVqeConfig.u32ChnNum = 1;
    stAiSetVqeConfig.bAecOpen = bAiEnableAec;
    stAiSetVqeConfig.bAgcOpen = bAiEnableAgc;
    stAiSetVqeConfig.bAnrOpen = bAiEnableNr;
    stAiSetVqeConfig.bEqOpen = bAiEnableEq;
    stAiSetVqeConfig.bHpfOpen = bAiEnableHpf;
    stAiSetVqeConfig.s32FrameSample = 128;
    stAiSetVqeConfig.s32WorkSampleRate = eAiSampleRate;
    memcpy(&stAiSetVqeConfig.stAecCfg, &stAecCfg, sizeof(MI_AI_AecConfig_t));
    memcpy(&stAiSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));
    memcpy(&stAiSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));
    memcpy(&stAiSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));
    memcpy(&stAiSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));
    memcpy(&node->par.stAiSetVqeConfig, &stAiSetVqeConfig, sizeof(stAiSetVqeConfig));

    node->par.bAiEnableResample = bAiEnableResample;
    node->par.eAiOutputResampleRate = eAiOutputResampleRate;

    memset(&stAiSetAencConfig, 0x0, sizeof(MI_AI_AencConfig_t));
    node->par.bAiEnableAenc = bAiEnableAenc;
    stAiSetAencConfig.eAencType = eAiAencType;
    memcpy(&node->par.stAiSetAencConfig, &stAiSetAencConfig, sizeof(stAiSetAencConfig));

#endif

    return 0;
}

int audio_ai_par_execute(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn)
{
    ai_list_node_t *node = get_node(AiDevId, AiChn);
    if (!node)
    {
        return -1;
    }
    return audio_ai_par_set(&node->par);
}

static int audio_ai_par_set(audio_ai_par_info_t *info)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_DEV AiDevId = info->AiDevId;
    MI_AI_CHN AiChn = info->AiChn;
    if (!info)
    {
        return -1;
    }

    if (info->bAiEnableAed)
    {
        ExecFunc(MI_AI_SetAedAttr(AiDevId, AiChn, &info->stAiSetAedConfig), MI_SUCCESS);
        ExecFunc(MI_AI_EnableAed(AiDevId, AiChn), MI_SUCCESS);
    }

    if (info->bAiEnableSsl)
    {

        ExecFunc(MI_AI_SetSslInitAttr(AiDevId, AiChn, &info->stSslInit), MI_SUCCESS);
        ExecFunc(MI_AI_SetSslConfigAttr(AiDevId, AiChn, &info->stSslConfig), MI_SUCCESS);
        ExecFunc(MI_AI_EnableSsl(AiDevId, AiChn), MI_SUCCESS);
    }

    if (info->bAiEnableBf)
    {
        ExecFunc(MI_AI_SetBfInitAttr(AiDevId, AiChn, &info->stBfInit), MI_SUCCESS);
        ExecFunc(MI_AI_SetBfConfigAttr(AiDevId, AiChn, &info->stBfConfig), MI_SUCCESS);
        if (info->bAiSetBfDoa)
        {
            ExecFunc(MI_AI_SetBfAngle(AiDevId, AiChn, info->s32AiBfDoa), MI_SUCCESS);
        }
        ExecFunc(MI_AI_EnableBf(AiDevId, AiChn), MI_SUCCESS);
    }

    if (info->bAiEnableVqe)
    {
        ExecFunc(MI_AI_SetVqeAttr(AiDevId, AiChn, info->AoDevId, info->AoChn, &info->stAiSetVqeConfig), MI_SUCCESS);
        ExecFunc(MI_AI_EnableVqe(AiDevId, AiChn), MI_SUCCESS);
    }

    if (info->bAiEnableResample)
    {
        ExecFunc(MI_AI_EnableReSmp(AiDevId, AiChn, info->eAiOutputResampleRate), MI_SUCCESS);
    }

    if (info->bAiEnableAenc)
    {
        ExecFunc(MI_AI_SetAencAttr(AiDevId, AiChn, &info->stAiSetAencConfig), MI_SUCCESS);
        ExecFunc(MI_AI_EnableAenc(AiDevId, AiChn), MI_SUCCESS);
    }
    return 0;
}

int audio_ai_par_close(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn)
{
    ai_list_node_t *node = get_node(AiDevId, AiChn);
    if (!node)
    {
        return -1;
    }
    if (node->par.bAiEnableAed)
    {
        ExecFunc(MI_AI_DisableAed(AiDevId, AiChn), MI_SUCCESS);
    }
    if (node->par.bAiEnableSsl)
    {
        ExecFunc(MI_AI_DisableSsl(AiDevId, AiChn), MI_SUCCESS);
    }
    if (node->par.bAiEnableBf)
    {
        ExecFunc(MI_AI_DisableBf(AiDevId, AiChn), MI_SUCCESS);
    }
    if (node->par.bAiEnableVqe)
    {
        ExecFunc(MI_AI_DisableVqe(AiDevId, AiChn), MI_SUCCESS);
    }
    if (node->par.bAiEnableAenc)
    {
        ExecFunc(MI_AI_DisableAenc(AiDevId, AiChn), MI_SUCCESS);
    }
    if (node->par.bAiEnableResample)
    {
        ExecFunc(MI_AI_DisableReSmp(AiDevId, AiChn), MI_SUCCESS);
    }
    delete_node(AiDevId, AiChn);
}

audio_ai_par_info_t *audio_ai_get_par(MI_AUDIO_DEV AiDevId, MI_AO_CHN AiChn)
{
    ai_list_node_t *node = get_node(AiDevId, AiChn);
    if (!node)
    {
        return NULL;
    }
    return &node->par;
}

static ai_list_node_t *create_node(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    ai_list_node_t *node;

    node = malloc(sizeof(ai_list_node_t));

    if (!node)
    {
        return NULL;
    }
    memset(node, 0, sizeof(ai_list_node_t));

    node->AiDevId = AiDevId;
    node->AiChn = AiChn;

    node->next = NULL;

    if (!ai_info.begin)
    {
        ai_info.begin = ai_info.end = node;
    }
    else
    {
        ai_info.end->next = node;
        ai_info.end = node;
    }

    return node;
}

static int delete_node(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    ai_list_node_t *cur_node = ai_info.begin;
    ai_list_node_t *prev_node = NULL;

    while (cur_node)
    {
        if (cur_node->AiDevId != AiDevId || AiChn != cur_node->AiChn)
        {
            prev_node = cur_node;
            cur_node = cur_node->next;
            continue;
        }

        if (cur_node == ai_info.begin)
        {
            ai_info.begin = cur_node->next;
        }

        if (cur_node == ai_info.end)
        {
            if (prev_node)
            {
                prev_node->next = NULL;
            }

            ai_info.end = prev_node;
        }

        if (prev_node)
        {
            prev_node->next = cur_node->next;
        }

        free(cur_node);
        return 0;
    }
    return -1;
}

static ai_list_node_t *get_node(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn)
{
    ai_list_node_t *cur_node = ai_info.begin;
    ai_list_node_t *list = NULL;

    while (cur_node)
    {
        if (cur_node->AiDevId == AiDevId && AiChn == cur_node->AiChn)
        {
            list = cur_node;
            break;
        }

        cur_node = cur_node->next;
    }

    return list;
}

#if EN_INI_PARSER

static int audio_ai_par_load_ini(const char *ini_path, audio_ai_par_info_t *info)
{
    int status;
    if (!ini_path || !info)
    {
        printf("%s %d Path or info is empty!!\r\n", __func__, __LINE__);
        return 0;
    }
    status = access(ini_path, F_OK);
    if (status == -1)
    {
        if (0 == create_default_ini_file(ini_path, INI_AI_PARAMETER))
        {
            return 0;
        }
        else
        {
            printf("warn: AO ini file does not exist, default created!  path: %s\r\n", ini_path);
        }
    }

    dictionary *ini;

    /* Some temporary variables to hold query results */
    int i, b;
    const char *s;

    ini = iniparser_load(ini_path);
    if (ini == NULL)
    {
        fprintf(stderr, "cannot parse file: %s\n", ini_path);
        return 0;
    }
    iniparser_dump(ini, stderr);

    /* Get AI attributes */

    b = iniparser_getboolean(ini, "AI:bAiEnableAed", 0);
    info->bAiEnableAed = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AI:bAiEnableSsl", 0);
    info->bAiEnableSsl = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AI:bAiEnableBf", 0);
    info->bAiEnableBf = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AI:bAiSetBfDoa", 0);
    info->bAiSetBfDoa = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AI:bAiEnableVqe", 0);
    info->bAiEnableVqe = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AI:bAiEnableResample", 0);
    info->bAiEnableResample = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AI:bAiEnableAenc", 0);
    info->bAiEnableAenc = b == 1 ? TRUE : FALSE;

    i = iniparser_getint(ini, "AI:s32AiBfDoa", 0);
    info->s32AiBfDoa = i;

    /* Get AED attributes */
    status = audio_ai_aed_load_ini(ini, &info->stAiSetAedConfig);
    if (status == 0)
    {
        fprintf(stderr, "AI AED ERROR\n");
        goto PAR_ERROR;
    }
    /* Get SSL attributes */
    status = audio_ai_ssl_load_ini(ini, &info->stSslInit, &info->stSslConfig);
    if (status == 0)
    {
        fprintf(stderr, "AI SSL ERROR\n");
        goto PAR_ERROR;
    }
    /* Get BF attributes */
    status = audio_ai_bf_load_ini(ini, &info->stBfInit, &info->stBfConfig);
    if (status == 0)
    {
        fprintf(stderr, "AI BF ERROR\n");
        goto PAR_ERROR;
    }
    /* Get VQE attributes */
    status = audio_ai_vqe_load_ini(ini, &info->stAiSetVqeConfig, &info->AoDevId, &info->AoChn);
    if (status == 0)
    {
        fprintf(stderr, "AI VQE ERROR\n");
        goto PAR_ERROR;
    }

    /* Get RATE attributes */
    status = audio_ai_rate_load_ini(ini, &info->eAiOutputResampleRate);
    if (status == 0)
    {
        fprintf(stderr, "AI RATE ERROR\n");
        goto PAR_ERROR;
    }
    /* Get AENC attributes */
    status = audio_ai_aenc_load_ini(ini, &info->stAiSetAencConfig);
    if (status == 0)
    {
        fprintf(stderr, "AI AENC ERROR\n");
        goto PAR_ERROR;
    }

    return 1;
PAR_SUCCESS:
    iniparser_freedict(ini);
    return 1;
PAR_ERROR:
    iniparser_freedict(ini);
    return 0;
}

static int audio_ai_vqe_load_ini(dictionary *ini, MI_AI_VqeConfig_t *cfg, MI_AUDIO_DEV *AoDevId, MI_AO_CHN *AoChn)
{
    int b, i, status;
    const char *s;
    assert(ini);
    assert(cfg);
    assert(AoDevId);
    assert(AoChn);

    /* Get VQE attributes */
    i = iniparser_getint(ini, "VQE:u32ChnNum", 1);
    cfg->u32ChnNum = i;

    b = iniparser_getboolean(ini, "VQE:bAecOpen", 0);
    cfg->bAecOpen = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "VQE:bAgcOpen", 0);
    cfg->bAgcOpen = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "VQE:bAnrOpen", 0);
    cfg->bAnrOpen = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "VQE:bEqOpen", 0);
    cfg->bEqOpen = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "VQE:bHpfOpen", 0);
    cfg->bHpfOpen = b == 1 ? TRUE : FALSE;

    i = iniparser_getint(ini, "VQE:s32FrameSample", 128);
    cfg->s32FrameSample = i;

    i = iniparser_getint(ini, "VQE:AoDevId", 128);
    *AoDevId = i;

    i = iniparser_getint(ini, "VQE:AoChn", 128);
    *AoChn = i;

    s = iniparser_getstring(ini, "VQE:s32WorkSampleRate", NULL);

    DEF_CMP_STR(MI_AUDIO_SampleRate_e)
    cmp_rate[] = {
        {"E_MI_AUDIO_SAMPLE_RATE_8000", E_MI_AUDIO_SAMPLE_RATE_8000},
        {"E_MI_AUDIO_SAMPLE_RATE_11025", E_MI_AUDIO_SAMPLE_RATE_11025},
        {"E_MI_AUDIO_SAMPLE_RATE_12000", E_MI_AUDIO_SAMPLE_RATE_12000},
        {"E_MI_AUDIO_SAMPLE_RATE_16000", E_MI_AUDIO_SAMPLE_RATE_16000},
        {"E_MI_AUDIO_SAMPLE_RATE_22050", E_MI_AUDIO_SAMPLE_RATE_22050},
        {"E_MI_AUDIO_SAMPLE_RATE_24000", E_MI_AUDIO_SAMPLE_RATE_24000},
        {"E_MI_AUDIO_SAMPLE_RATE_32000", E_MI_AUDIO_SAMPLE_RATE_32000},
        {"E_MI_AUDIO_SAMPLE_RATE_44100", E_MI_AUDIO_SAMPLE_RATE_44100},
        {"E_MI_AUDIO_SAMPLE_RATE_48000", E_MI_AUDIO_SAMPLE_RATE_48000},
        {"E_MI_AUDIO_SAMPLE_RATE_96000", E_MI_AUDIO_SAMPLE_RATE_96000},
        {"E_MI_AUDIO_SAMPLE_RATE_INVALID", E_MI_AUDIO_SAMPLE_RATE_INVALID}};

    cfg->s32WorkSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
    int ii;
    int size = sizeof(cmp_rate) / sizeof(DEF_CMP_STR(MI_AUDIO_SampleRate_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_rate[ii].str, s))
        {
            cfg->s32WorkSampleRate = cmp_rate[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "VQE:s32WorkSampleRate ERROR\n");
        return 0;
    }
    /* Get AEC attributes */
    status = audio_ai_aec_load_ini(ini, &cfg->stAecCfg);
    if (status == 0)
    {
        fprintf(stderr, "AI HPF ERROR\n");
        return 0;
    }
    /* Get HPF attributes */
    status = audio_ai_hpf_load_ini(ini, &cfg->stHpfCfg);
    if (status == 0)
    {
        fprintf(stderr, "AI HPF ERROR\n");
        return 0;
    }
    /* Get ANR attributes */
    status = audio_ai_anr_load_ini(ini, &cfg->stAnrCfg);
    if (status == 0)
    {
        fprintf(stderr, "AI ANR ERROR\n");
        return 0;
    }
    /* Get AGC attributes */
    status = audio_ai_agc_load_ini(ini, &cfg->stAgcCfg);
    if (status == 0)
    {
        fprintf(stderr, "AI AGC ERROR\n");
        return 0;
    }
    /* Get EQ attributes */
    status = audio_ai_eq_load_ini(ini, &cfg->stEqCfg);
    if (status == 0)
    {
        fprintf(stderr, "AI EQ ERROR\n");
        return 0;
    }

    return 1;
}
static int audio_ai_aed_load_ini(dictionary *ini, MI_AI_AedConfig_t *cfg)
{
    int i, b;
    const char *s;
    assert(ini);
    assert(cfg);
    b = iniparser_getboolean(ini, "AED:bEnableNr", 0);
    cfg->bEnableNr = b == 1 ? TRUE : FALSE;

    i = iniparser_getint(ini, "AED:s32OperatingPoint", -5);
    cfg->s32OperatingPoint = i;

    i = iniparser_getint(ini, "AED:s32VadThresholdDb", -40);
    cfg->s32VadThresholdDb = i;

    i = iniparser_getint(ini, "AED:s32LsdThresholdDb", -15);
    cfg->s32LsdThresholdDb = i;

    int ii, size;
    s = iniparser_getstring(ini, "AED:eSensitivity", NULL);
    DEF_CMP_STR(MI_AUDIO_AedSensitivity_e)
    cmp[] = {
        {"E_MI_AUDIO_AED_SEN_LOW", E_MI_AUDIO_AED_SEN_LOW},
        {"E_MI_AUDIO_AED_SEN_MID", E_MI_AUDIO_AED_SEN_MID},
        {"E_MI_AUDIO_AED_SEN_HIGH", E_MI_AUDIO_AED_SEN_HIGH},
        {"E_MI_AUDIO_AED_SEN_INVALID", E_MI_AUDIO_AED_SEN_INVALID}};

    cfg->eSensitivity = E_MI_AUDIO_AED_SEN_HIGH;

    size = sizeof(cmp) / sizeof(DEF_CMP_STR(MI_AUDIO_AedSensitivity_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp[ii].str, s))
        {
            cfg->eSensitivity = cmp[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "AED:eSensitivity ERROR\n");
        return 0;
    }
    return 1;
}
static int audio_ai_ssl_load_ini(dictionary *ini, MI_AI_SslInitAttr_t *attr, MI_AI_SslConfigAttr_t *cfg)
{
    int i, b;
    assert(ini);
    assert(cfg);
    b = iniparser_getboolean(ini, "SSL:bBfMode", 0);
    attr->bBfMode = b == 1 ? TRUE : FALSE;

    i = iniparser_getint(ini, "SSL:s32Temperature", 25);
    cfg->s32Temperature = i;

    i = iniparser_getint(ini, "SSL:s32NoiseGateDbfs", -40);
    cfg->s32NoiseGateDbfs = i;

    i = iniparser_getint(ini, "SSL:s32DirectionFrameNum", -300);
    cfg->s32DirectionFrameNum = i;

    return 1;
}
static int audio_ai_bf_load_ini(dictionary *ini, MI_AI_BfInitAttr_t *attr, MI_AI_BfConfigAttr_t *cfg)
{
    int i, b;
    const char *s;
    double d;
    assert(ini);
    assert(cfg);

    i = iniparser_getint(ini, "BF:u32MicDistance", 0);
    attr->u32MicDistance = i;

    i = iniparser_getint(ini, "BF:u32ChanCnt", 1);
    attr->u32ChanCnt = i;

    i = iniparser_getint(ini, "BF:s32Temperature", 25);
    cfg->s32Temperature = i;

    i = iniparser_getint(ini, "BF:s32NoiseGateDbfs", -40);
    cfg->s32NoiseGateDbfs = i;

    i = iniparser_getint(ini, "BF:s32NoiseSupressionMode", 8);
    cfg->s32NoiseSupressionMode = i;

    i = iniparser_getint(ini, "BF:s32NoiseEstimation", 1);
    cfg->s32NoiseEstimation = i;

    d = iniparser_getdouble(ini, "BF:s32NoiseEstimation", 0.7);
    cfg->outputGain = d;

    return 1;
}
static int audio_ai_rate_load_ini(dictionary *ini, MI_AUDIO_SampleRate_e *ae)
{
    const char *s;
    assert(ini);
    assert(ae);
    s = iniparser_getstring(ini, "RATE:eAiOutputResampleRate", NULL);
    DEF_CMP_STR(MI_AUDIO_SampleRate_e)
    cmp_rate[] = {
        {"E_MI_AUDIO_SAMPLE_RATE_8000", E_MI_AUDIO_SAMPLE_RATE_8000},
        {"E_MI_AUDIO_SAMPLE_RATE_11025", E_MI_AUDIO_SAMPLE_RATE_11025},
        {"E_MI_AUDIO_SAMPLE_RATE_12000", E_MI_AUDIO_SAMPLE_RATE_12000},
        {"E_MI_AUDIO_SAMPLE_RATE_16000", E_MI_AUDIO_SAMPLE_RATE_16000},
        {"E_MI_AUDIO_SAMPLE_RATE_22050", E_MI_AUDIO_SAMPLE_RATE_22050},
        {"E_MI_AUDIO_SAMPLE_RATE_24000", E_MI_AUDIO_SAMPLE_RATE_24000},
        {"E_MI_AUDIO_SAMPLE_RATE_32000", E_MI_AUDIO_SAMPLE_RATE_32000},
        {"E_MI_AUDIO_SAMPLE_RATE_44100", E_MI_AUDIO_SAMPLE_RATE_44100},
        {"E_MI_AUDIO_SAMPLE_RATE_48000", E_MI_AUDIO_SAMPLE_RATE_48000},
        {"E_MI_AUDIO_SAMPLE_RATE_96000", E_MI_AUDIO_SAMPLE_RATE_96000},
        {"E_MI_AUDIO_SAMPLE_RATE_INVALID", E_MI_AUDIO_SAMPLE_RATE_INVALID}};

    *ae = E_MI_AUDIO_SAMPLE_RATE_INVALID;
    int ii;
    int size = sizeof(cmp_rate) / sizeof(DEF_CMP_STR(MI_AUDIO_SampleRate_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_rate[ii].str, s))
        {
            *ae = cmp_rate[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "RATE:eAiOutputResampleRate ERROR\n");
        return 0;
    }
    return 1;
}
static int audio_ai_aenc_load_ini(dictionary *ini, MI_AI_AencConfig_t *cfg)
{
    const char *s;
    int ii, size;
    assert(ini);
    assert(cfg);
    MI_AUDIO_AencType_e eAencType;
    MI_AUDIO_SampleRate_e eSamplerate;
    MI_AUDIO_SoundMode_e eSoundmode;
    MI_AUDIO_G726Mode_e eG726Mode;

    s = iniparser_getstring(ini, "AENC:eAencType", NULL);

    DEF_CMP_STR(MI_AUDIO_AencType_e)
    cmp_type[] = {
        {"E_MI_AUDIO_AENC_TYPE_G711A", E_MI_AUDIO_AENC_TYPE_G711A},
        {"E_MI_AUDIO_AENC_TYPE_G711U", E_MI_AUDIO_AENC_TYPE_G711U},
        {"E_MI_AUDIO_AENC_TYPE_G726", E_MI_AUDIO_AENC_TYPE_G726},
        {"E_MI_AUDIO_AENC_TYPE_INVALID", E_MI_AUDIO_AENC_TYPE_INVALID}};

    size = sizeof(cmp_type) / sizeof(DEF_CMP_STR(MI_AUDIO_AencType_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_type[ii].str, s))
        {
            eAencType = cmp_type[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "AENC:eAencType ERROR\n");
        return 0;
    }

    s = iniparser_getstring(ini, "AENC:eSamplerate", NULL);

    DEF_CMP_STR(MI_AUDIO_SampleRate_e)
    cmp_eSamplerate[] = {
        {"E_MI_AUDIO_SAMPLE_RATE_8000", E_MI_AUDIO_SAMPLE_RATE_8000},
        {"E_MI_AUDIO_SAMPLE_RATE_11025", E_MI_AUDIO_SAMPLE_RATE_11025},
        {"E_MI_AUDIO_SAMPLE_RATE_12000", E_MI_AUDIO_SAMPLE_RATE_12000},
        {"E_MI_AUDIO_SAMPLE_RATE_16000", E_MI_AUDIO_SAMPLE_RATE_16000},
        {"E_MI_AUDIO_SAMPLE_RATE_22050", E_MI_AUDIO_SAMPLE_RATE_22050},
        {"E_MI_AUDIO_SAMPLE_RATE_24000", E_MI_AUDIO_SAMPLE_RATE_24000},
        {"E_MI_AUDIO_SAMPLE_RATE_32000", E_MI_AUDIO_SAMPLE_RATE_32000},
        {"E_MI_AUDIO_SAMPLE_RATE_44100", E_MI_AUDIO_SAMPLE_RATE_44100},
        {"E_MI_AUDIO_SAMPLE_RATE_48000", E_MI_AUDIO_SAMPLE_RATE_48000},
        {"E_MI_AUDIO_SAMPLE_RATE_96000", E_MI_AUDIO_SAMPLE_RATE_96000},
        {"E_MI_AUDIO_SAMPLE_RATE_INVALID", E_MI_AUDIO_SAMPLE_RATE_INVALID}};

    size = sizeof(cmp_eSamplerate) / sizeof(DEF_CMP_STR(MI_AUDIO_SampleRate_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_eSamplerate[ii].str, s))
        {
            eSamplerate = cmp_eSamplerate[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "AENC:eSamplerate ERROR\n");
        return 0;
    }

    s = iniparser_getstring(ini, "AENC:eSoundmode", NULL);

    DEF_CMP_STR(MI_AUDIO_SoundMode_e)
    cmp_eSoundmode[] = {
        {"E_MI_AUDIO_SOUND_MODE_MONO", E_MI_AUDIO_SOUND_MODE_MONO},
        {"E_MI_AUDIO_SOUND_MODE_STEREO", E_MI_AUDIO_SOUND_MODE_STEREO},
        {"E_MI_AUDIO_SOUND_MODE_QUEUE", E_MI_AUDIO_SOUND_MODE_QUEUE},
        {"E_MI_AUDIO_SOUND_MODE_MAX", E_MI_AUDIO_SOUND_MODE_MAX}};

    size = sizeof(cmp_eSoundmode) / sizeof(DEF_CMP_STR(MI_AUDIO_SoundMode_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_eSoundmode[ii].str, s))
        {
            eSoundmode = cmp_eSoundmode[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "AENC:eSoundmode ERROR\n");
        return 0;
    }

    s = iniparser_getstring(ini, "AENC:eG726Mode", NULL);

    DEF_CMP_STR(MI_AUDIO_G726Mode_e)
    cmp_eG726Mode[] = {
        {"E_MI_AUDIO_G726_MODE_16", E_MI_AUDIO_G726_MODE_16},
        {"E_MI_AUDIO_G726_MODE_24", E_MI_AUDIO_G726_MODE_24},
        {"E_MI_AUDIO_G726_MODE_32", E_MI_AUDIO_G726_MODE_32},
        {"E_MI_AUDIO_G726_MODE_40", E_MI_AUDIO_G726_MODE_40},
        {"E_MI_AUDIO_G726_MODE_INVALID", E_MI_AUDIO_G726_MODE_INVALID}};

    size = sizeof(cmp_eG726Mode) / sizeof(DEF_CMP_STR(MI_AUDIO_G726Mode_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_eG726Mode[ii].str, s))
        {
            eG726Mode = cmp_eG726Mode[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "AENC:eG726Mode ERROR\n");
        return 0;
    }

    cfg->eAencType = eAencType;
    if (eAencType == E_MI_AUDIO_AENC_TYPE_G726)
    {
        cfg->stAencG726Cfg.eG726Mode = eG726Mode;
        cfg->stAencG726Cfg.eSamplerate = eSamplerate;
        cfg->stAencG726Cfg.eSoundmode = eSoundmode;
    }
    else
    {
        cfg->stAencG711Cfg.eSamplerate = eSamplerate;
        cfg->stAencG711Cfg.eSoundmode = eSoundmode;
    }

    return 1;
}
static int audio_ai_aec_load_ini(dictionary *ini, MI_AI_AecConfig_t *cfg)
{
    int i, b, ii;
    const char *s;
    assert(ini);
    assert(cfg);

    i = iniparser_getint(ini, "AEC:s16DelaySample", 0);
    cfg->s16DelaySample = i;

    b = iniparser_getboolean(ini, "AEC:bComfortNoiseEnable", 0);
    cfg->bComfortNoiseEnable = b == 1 ? TRUE : FALSE;

    /*u32AecSupfreq*/
    s = iniparser_getstring(ini, "AEC:u32AecSupfreq", NULL);
    if (!s)
    {
        fprintf(stderr, "AEC:u32AecSupfreq ERROR\n");
        return 0;
    }
    char *p;
    char *str, *str1;
    int num = 0;
    str = strdup(s);
    if (!str)
    {
        return 0;
    }

    str1 = strstr(str, "{");
    if (!str1)
    {
        free(str);
        return 0;
    }
    str1 += 1;
    ii = 0;
    while ((p = strsep(&str1, ",")))
    {
        num = atoi(p);
        cfg->u32AecSupfreq[ii] = num;
        ii++;
        if (ii >= 6)
        {
            break;
        }
    }
    //剩下的，写最后一个数据
    for (; ii < 6; ii++)
    {
        cfg->u32AecSupfreq[ii] = num;
    }
    free(str);

    /*u32AecSupIntensity*/
    s = iniparser_getstring(ini, "AEC:u32AecSupIntensity", NULL);
    if (!s)
    {
        fprintf(stderr, "AEC:u32AecSupIntensity ERROR\n");
        return 0;
    }

    str = strdup(s);
    if (!str)
    {
        return 0;
    }

    str1 = strstr(str, "{");
    if (!str1)
    {
        free(str);
        return 0;
    }
    str1 += 1;
    ii = 0;
    while ((p = strsep(&str1, ",")))
    {
        num = atoi(p);
        cfg->u32AecSupIntensity[ii] = num;
        ii++;
        if (ii >= 7)
        {
            break;
        }
    }
    //剩下的，写最后一个数据
    for (; ii < 7; ii++)
    {
        cfg->u32AecSupIntensity[ii] = num;
    }
    free(str);

    return 1;
}
static int audio_ao_vqe_mode_load_ini(dictionary *ini, const char *str, MI_AUDIO_AlgorithmMode_e *e)
{
    const char *s;
    int ii, size;
    assert(ini);
    s = iniparser_getstring(ini, str, NULL);
    DEF_CMP_STR(MI_AUDIO_AlgorithmMode_e)
    cmp[] = {
        {"E_MI_AUDIO_ALGORITHM_MODE_DEFAULT", E_MI_AUDIO_ALGORITHM_MODE_DEFAULT},
        {"E_MI_AUDIO_ALGORITHM_MODE_USER", E_MI_AUDIO_ALGORITHM_MODE_USER},
        {"E_MI_AUDIO_ALGORITHM_MODE_MUSIC", E_MI_AUDIO_ALGORITHM_MODE_MUSIC},
        {"E_MI_AUDIO_ALGORITHM_MODE_INVALID", E_MI_AUDIO_ALGORITHM_MODE_INVALID}};

    *e = E_MI_AUDIO_ALGORITHM_MODE_USER;

    size = sizeof(cmp) / sizeof(DEF_CMP_STR(MI_AUDIO_AlgorithmMode_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp[ii].str, s))
        {
            *e = cmp[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "%s ERROR\n", str);
        return 0;
    }

    return 1;
}

static int audio_ai_agc_load_ini(dictionary *ini, MI_AUDIO_AgcConfig_t *cfg)
{
    const char *s;
    int i, ii, size, status;
    assert(ini);
    assert(cfg);
    status = audio_ao_vqe_mode_load_ini(ini, "AGC:eMode", &cfg->eMode);
    if (status == 0)
    {
        return 0;
    }

    i = iniparser_getint(ini, "AGC:s32NoiseGateDb", -60);
    cfg->s32NoiseGateDb = i;
    i = iniparser_getint(ini, "AGC:s32TargetLevelDb", -3);
    cfg->s32TargetLevelDb = i;
    i = iniparser_getint(ini, "AGC:stAgcGainInfo.s32GainInit", 0);
    cfg->stAgcGainInfo.s32GainInit = i;
    i = iniparser_getint(ini, "AGC:stAgcGainInfo.s32GainMax", 20);
    cfg->stAgcGainInfo.s32GainMax = i;
    i = iniparser_getint(ini, "AGC:stAgcGainInfo.s32GainMin", 0);
    cfg->stAgcGainInfo.s32GainMin = i;
    i = iniparser_getint(ini, "AGC:u32AttackTime", 1);
    cfg->u32AttackTime = i;
    i = iniparser_getint(ini, "AGC:u32DropGainMax", 12);
    cfg->u32DropGainMax = i;
    i = iniparser_getint(ini, "AGC:u32NoiseGateAttenuationDb", 0);
    cfg->u32NoiseGateAttenuationDb = i;
    i = iniparser_getint(ini, "AGC:u32ReleaseTime", 3);
    cfg->u32ReleaseTime = i;

    /*s16Compression_ratio_input*/
    s = iniparser_getstring(ini, "AGC:s16Compression_ratio_input", NULL);
    if (!s)
    {
        fprintf(stderr, "AGC:s16Compression_ratio_input ERROR\n");
        return 0;
    }
    char *p;
    char *str, *str1;
    int num = 0;
    str = strdup(s);
    if (!str)
    {
        return 0;
    }

    str1 = strstr(str, "{");
    if (!str1)
    {
        free(str);
        return 0;
    }
    str1 += 1;
    ii = 0;
    while ((p = strsep(&str1, ",")))
    {
        num = atoi(p);
        cfg->s16Compression_ratio_input[ii] = num;
        ii++;
        if (ii >= 5)
        {
            break;
        }
    }
    //剩下的，写最后一个数据
    for (; ii < 5; ii++)
    {
        cfg->s16Compression_ratio_input[ii] = num;
    }
    free(str);

    /*s16Compression_ratio_output*/
    s = iniparser_getstring(ini, "AGC:s16Compression_ratio_output", NULL);
    if (!s)
    {
        fprintf(stderr, "AGC:s16Compression_ratio_output ERROR\n");
        return 0;
    }

    str = strdup(s);
    if (!str)
    {
        return 0;
    }

    str1 = strstr(str, "{");
    if (!str1)
    {
        free(str);
        return 0;
    }
    str1 += 1;
    ii = 0;
    while ((p = strsep(&str1, ",")))
    {
        num = atoi(p);
        cfg->s16Compression_ratio_output[ii] = num;
        ii++;
        if (ii >= 5)
        {
            break;
        }
    }
    //剩下的，写最后一个数据
    for (; ii < 5; ii++)
    {
        cfg->s16Compression_ratio_output[ii] = num;
    }
    free(str);

    return 1;
}

static int audio_ai_anr_load_ini(dictionary *ini, MI_AUDIO_AnrConfig_t *cfg)
{
    const char *s;
    int i, ii, size, status;
    assert(ini);
    assert(cfg);
    status = audio_ao_vqe_mode_load_ini(ini, "ANR:eMode", &cfg->eMode);
    if (status == 0)
    {
        return 0;
    }

    i = iniparser_getint(ini, "ANR:u32NrIntensity", -1);
    if (i == -1)
    {
        fprintf(stderr, "ANR:u32NrIntensity ERROR\n");
        return 0;
    }
    else
    {
        cfg->u32NrIntensity = i;
    }

    i = iniparser_getint(ini, "ANR:u32NrSmoothLevel", -1);
    if (i == -1)
    {
        fprintf(stderr, "ANR:u32NrSmoothLevel ERROR\n");
        return 0;
    }
    else
    {
        cfg->u32NrSmoothLevel = i;
    }

    s = iniparser_getstring(ini, "ANR:eNrSpeed", NULL);
    DEF_CMP_STR(MI_AUDIO_NrSpeed_e)
    cmp1[] = {
        {"E_MI_AUDIO_NR_SPEED_LOW", E_MI_AUDIO_NR_SPEED_LOW},
        {"E_MI_AUDIO_NR_SPEED_MID", E_MI_AUDIO_NR_SPEED_MID},
        {"E_MI_AUDIO_NR_SPEED_HIGH", E_MI_AUDIO_NR_SPEED_HIGH}};

    cfg->eNrSpeed = E_MI_AUDIO_NR_SPEED_MID;

    size = sizeof(cmp1) / sizeof(DEF_CMP_STR(MI_AUDIO_HpfFreq_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp1[ii].str, s))
        {
            cfg->eNrSpeed = cmp1[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "ANR:eNrSpeed ERROR\n");
        return 0;
    }

    return 1;
}

static int audio_ai_eq_load_ini(dictionary *ini, MI_AUDIO_EqConfig_t *cfg)
{
    const char *s;
    int ii, size, status;
    assert(ini);
    assert(cfg);
    status = audio_ao_vqe_mode_load_ini(ini, "EQ:eMode", &cfg->eMode);
    if (status == 0)
    {
        return 0;
    }
    s = iniparser_getstring(ini, "EQ:s16EqGainDb", NULL);
    if (!s)
    {
        fprintf(stderr, "EQ:s16EqGainDb ERROR\n");
        return 0;
    }

    char *p;
    char *str, *str1;
    int num = 0;
    str = strdup(s);
    if (!str)
    {
        return 0;
    }

    str1 = strstr(str, "{");
    if (!str1)
    {
        free(str);
        return 0;
    }
    str1 += 1;
    ii = 0;
    while ((p = strsep(&str1, ",")))
    {
        num = atoi(p);
        cfg->s16EqGainDb[ii] = num;
        ii++;
        if (ii >= 129)
        {
            break;
        }
    }
    //剩下的，写最后一个数据
    for (; ii < 129; ii++)
    {
        cfg->s16EqGainDb[ii] = num;
    }

    // printf("s16EqGainDb:{\n");
    // for (ii = 0; ii < 129; ii++)
    // {
    //     printf("%d,", cfg->s16EqGainDb[ii]);
    // }
    // printf("}\n");

    free(str);

    return 1;
}

static int audio_ai_hpf_load_ini(dictionary *ini, MI_AUDIO_HpfConfig_t *cfg)
{
    const char *s;
    int ii, size, status;
    assert(ini);
    assert(cfg);
    status = audio_ao_vqe_mode_load_ini(ini, "HPF:eMode", &cfg->eMode);
    if (status == 0)
    {
        return 0;
    }

    s = iniparser_getstring(ini, "HPF:eHpfFreq", NULL);
    DEF_CMP_STR(MI_AUDIO_HpfFreq_e)
    cmp1[] = {
        {"E_MI_AUDIO_HPF_FREQ_80", E_MI_AUDIO_HPF_FREQ_80},
        {"E_MI_AUDIO_HPF_FREQ_120", E_MI_AUDIO_HPF_FREQ_120},
        {"E_MI_AUDIO_HPF_FREQ_150", E_MI_AUDIO_HPF_FREQ_150},
        {"E_MI_AUDIO_HPF_FREQ_INVALID", E_MI_AUDIO_HPF_FREQ_INVALID}};

    cfg->eHpfFreq = E_MI_AUDIO_HPF_FREQ_150;

    size = sizeof(cmp1) / sizeof(DEF_CMP_STR(MI_AUDIO_HpfFreq_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp1[ii].str, s))
        {
            cfg->eHpfFreq = cmp1[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "HPF:eHpfFreq ERROR\n");
        return 0;
    }

    return 1;
}

static int create_default_ini_file(const char *ini_name, const char *def)
{
    FILE *ini;
    assert(ini_name);
    assert(def);

    if ((ini = fopen(ini_name, "w")) == NULL)
    {
        fprintf(stderr, "iniparser: cannot create %s\n", ini_name);
        return 0;
    }

    fprintf(ini, def);

    fflush(ini);
    fsync(fileno(ini));
    fclose(ini);
    return 1;
}

#endif
