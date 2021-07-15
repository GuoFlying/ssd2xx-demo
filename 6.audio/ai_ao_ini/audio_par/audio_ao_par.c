/*********************************************************************************
  *FileName:  audio_ao_par.c
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-23
**********************************************************************************/
#include "audio_ao_par.h"
#include <stdlib.h>
#include <assert.h>

typedef struct ao_list_node_t_
{
    MI_AUDIO_DEV AoDevId;
    MI_AO_CHN AoChn;

    audio_ao_par_info_t par;

    struct ao_list_node_t_ *next;
} ao_list_node_t;

typedef struct ao_info_t_
{
    ao_list_node_t *begin;
    ao_list_node_t *end;
} ao_info_t;

static ao_info_t ao_info = {
    .begin = NULL,
    .end = NULL};

static ao_list_node_t *create_node(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
static ao_list_node_t *get_node(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
static int delete_node(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
static int audio_ao_par_set(audio_ao_par_info_t *info);

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
static int audio_ao_par_load_ini(const char *ini_path, audio_ao_par_info_t *info);
static int audio_ao_agc_load_ini(dictionary *ini, MI_AUDIO_AgcConfig_t *cfg);
static int audio_ao_anr_load_ini(dictionary *ini, MI_AUDIO_AnrConfig_t *cfg);
static int audio_ao_eq_load_ini(dictionary *ini, MI_AUDIO_EqConfig_t *cfg);
static int audio_ao_hpf_load_ini(dictionary *ini, MI_AUDIO_HpfConfig_t *cfg);

#else

static MI_S32 s32AoVolume = -5;

static MI_BOOL bAoEnableVqe = TRUE;
static MI_AUDIO_SampleRate_e eAoOutSampleRate = E_MI_AUDIO_SAMPLE_RATE_8000;

static MI_BOOL bAoEnableHpf = FALSE;
static MI_BOOL bAoEnableAgc = TRUE;
static MI_BOOL bAoEnableNr = TRUE;
static MI_BOOL bAoEnableEq = TRUE;

static MI_BOOL bAoMute = FALSE;

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

#endif

int audio_ao_par_analysis(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, const char *ini_path)
{
    ao_list_node_t *node = get_node(AoDevId, AoChn);
    if (!node)
    {
        node = create_node(AoDevId, AoChn);
    }

    if (!node)
    {
        return -1;
    }

    node->par.AoDevId = AoDevId;
    node->par.AoChn = AoChn;

#if EN_INI_PARSER
    if (0 == audio_ao_par_load_ini(ini_path, &node->par))
    {
        return 0;
    }

#else
    MI_AO_VqeConfig_t stAoSetVqeConfig;
    stAoSetVqeConfig.bAgcOpen = bAoEnableAgc;
    stAoSetVqeConfig.bAnrOpen = bAoEnableNr;
    stAoSetVqeConfig.bEqOpen = bAoEnableEq;
    stAoSetVqeConfig.bHpfOpen = bAoEnableHpf;
    stAoSetVqeConfig.s32FrameSample = 128;
    stAoSetVqeConfig.s32WorkSampleRate = eAoOutSampleRate;
    memcpy(&stAoSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));
    memcpy(&stAoSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));
    memcpy(&stAoSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));
    memcpy(&stAoSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));

    node->par.s32AoVolume = s32AoVolume;
    node->par.bAoMute = bAoMute;

    node->par.bAoEnableVqe = bAoEnableVqe;
    memcpy(&node->par.stAoSetVqeConfig, &stAoSetVqeConfig, sizeof(MI_AO_VqeConfig_t));
#endif

    return 0;
}

int audio_ao_par_execute(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    int ret;
    ao_list_node_t *node = get_node(AoDevId, AoChn);
    if (!node)
    {
       return -1;
    }
    ret = audio_ao_par_set(&node->par);
    return ret;
}
audio_ao_par_info_t *audio_ao_get_par(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    ao_list_node_t *node = get_node(AoDevId, AoChn);
    if (!node)
    {
       return NULL;
    }
    return &node->par;
}
int audio_ao_par_close(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    ao_list_node_t *node = get_node(AoDevId, AoChn);
    if (!node)
    {
       return -1;
    }
    if (node->par.bAoEnableVqe)
    {
        ExecFunc(MI_AO_DisableVqe(AoDevId, AoChn), MI_SUCCESS);
    }
    return 0;
}

static ao_list_node_t *create_node(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    ao_list_node_t *node;

    node = malloc(sizeof(ao_list_node_t));

    if (!node)
    {
        return NULL;
    }
    memset(node, 0, sizeof(ao_list_node_t));

    node->AoDevId = AoDevId;
    node->AoChn = AoChn;

    node->next = NULL;

    if (!ao_info.begin)
    {
        ao_info.begin = ao_info.end = node;
    }
    else
    {
        ao_info.end->next = node;
        ao_info.end = node;
    }

    return node;
}

static int delete_node(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    ao_list_node_t *cur_node = ao_info.begin;
    ao_list_node_t *prev_node = NULL;

    while (cur_node)
    {
        if (cur_node->AoDevId != AoDevId || AoChn != cur_node->AoChn)
        {
            prev_node = cur_node;
            cur_node = cur_node->next;
            continue;
        }

        if (cur_node == ao_info.begin)
        {
            ao_info.begin = cur_node->next;
        }

        if (cur_node == ao_info.end)
        {
            if (prev_node)
            {
                prev_node->next = NULL;
            }

            ao_info.end = prev_node;
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

static ao_list_node_t *get_node(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn)
{
    ao_list_node_t *cur_node = ao_info.begin;
    ao_list_node_t *list = NULL;

    while (cur_node)
    {
        if (cur_node->AoDevId == AoDevId && AoChn == cur_node->AoChn)
        {
            list = cur_node;
            break;
        }

        cur_node = cur_node->next;
    }

    return list;
}

#if EN_INI_PARSER

static int audio_ao_par_load_ini(const char *ini_path, audio_ao_par_info_t *info)
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
        if (0 == create_default_ini_file(ini_path, INI_AO_PARAMETER))
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

    /* Get AO attributes */

    i = iniparser_getint(ini, "AO:s32AoVolume", 0);
    info->s32AoVolume = i;

    b = iniparser_getboolean(ini, "AO:bAoMute", 0);
    info->bAoMute = b == 1 ? TRUE : FALSE;

    b = iniparser_getboolean(ini, "AO:bAoEnableVqe", 0);
    info->bAoEnableVqe = b == 1 ? TRUE : FALSE;
    bAoEnableVqe = info->bAoEnableVqe;

    if (FALSE == info->bAoEnableVqe)
    {
        goto PAR_SUCCESS;
    }

    MI_AO_VqeConfig_t *vqe_info = &info->stAoSetVqeConfig;

    /* Get VQE attributes */
    b = iniparser_getboolean(ini, "VQE:bHpfOpen", 0);
    vqe_info->bHpfOpen = b == 1 ? TRUE : FALSE;
    // if (TRUE == vqe_info->bHpfOpen)
    // {//不使能时，一样要加载其他参数，不然MI接口报错，。。。
    //     status = audio_ao_hpf_load_ini(ini, &vqe_info->stHpfCfg);
    //     if (status == 0)
    //     {
    //         fprintf(stderr, "HPF ERROR\n");
    //         goto PAR_ERROR;
    //     }
    // }

    status = audio_ao_hpf_load_ini(ini, &vqe_info->stHpfCfg);
    if (status == 0)
    {
        fprintf(stderr, "HPF ERROR\n");
        goto PAR_ERROR;
    }

    b = iniparser_getboolean(ini, "VQE:bEqOpen", 0);
    vqe_info->bEqOpen = b == 1 ? TRUE : FALSE;

    status = audio_ao_eq_load_ini(ini, &vqe_info->stEqCfg);
    if (status == 0)
    {
        fprintf(stderr, "EQ ERROR\n");
        goto PAR_ERROR;
    }

    b = iniparser_getboolean(ini, "VQE:bAnrOpen", 0);
    vqe_info->bAnrOpen = b == 1 ? TRUE : FALSE;

    status = audio_ao_anr_load_ini(ini, &vqe_info->stAnrCfg);
    if (status == 0)
    {
        fprintf(stderr, "ANR ERROR\n");
        goto PAR_ERROR;
    }

    b = iniparser_getboolean(ini, "VQE:bAgcOpen", 0);
    vqe_info->bAgcOpen = b == 1 ? TRUE : FALSE;

    status = audio_ao_agc_load_ini(ini, &vqe_info->stAgcCfg);
    if (status == 0)
    {
        fprintf(stderr, "AGC ERROR\n");
        goto PAR_ERROR;
    }

    i = iniparser_getint(ini, "VQE:s32FrameSample", 128);
    vqe_info->s32FrameSample = i;

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

    vqe_info->s32WorkSampleRate = E_MI_AUDIO_SAMPLE_RATE_INVALID;
    int ii;
    int size = sizeof(cmp_rate) / sizeof(DEF_CMP_STR(MI_AUDIO_SampleRate_e));
    for (ii = 0; s && ii < size; ii++)
    {
        if (0 == strcmp(cmp_rate[ii].str, s))
        {
            vqe_info->s32WorkSampleRate = cmp_rate[ii].e;
            break;
        }
    }
    if (ii == size || !s)
    {
        fprintf(stderr, "VQE:s32WorkSampleRate ERROR\n");
        goto PAR_ERROR;
    }

PAR_SUCCESS:
    iniparser_freedict(ini);
    return 1;
PAR_ERROR:
    iniparser_freedict(ini);
    return 0;
}

static int audio_ao_vqe_mode_load_ini(dictionary *ini, const char *str, MI_AUDIO_AlgorithmMode_e *e)
{
    const char *s;
    int ii, size;

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

static int audio_ao_agc_load_ini(dictionary *ini, MI_AUDIO_AgcConfig_t *cfg)
{
    const char *s;
    int i, ii, size, status;
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

static int audio_ao_anr_load_ini(dictionary *ini, MI_AUDIO_AnrConfig_t *cfg)
{
    const char *s;
    int i, ii, size, status;
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

static int audio_ao_eq_load_ini(dictionary *ini, MI_AUDIO_EqConfig_t *cfg)
{
    const char *s;
    int ii, size, status;
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

static int audio_ao_hpf_load_ini(dictionary *ini, MI_AUDIO_HpfConfig_t *cfg)
{
    const char *s;
    int ii, size, status;
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

static int audio_ao_par_set(audio_ao_par_info_t *info)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_AUDIO_DEV AoDevId = info->AoDevId;
    MI_AO_CHN AoChn = info->AoChn;
    MI_BOOL bInitVqe = FALSE;

    if (info->bAoEnableVqe)
    {
        ExecFuncNoExit(MI_AO_SetVqeAttr(AoDevId, AoChn, &info->stAoSetVqeConfig), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
            goto DISABLE_ALG;
        }

        ExecFuncNoExit(MI_AO_EnableVqe(AoDevId, AoChn), MI_SUCCESS, s32Ret);
        if (MI_SUCCESS != s32Ret)
        {
            goto DISABLE_ALG;
        }
        bInitVqe = TRUE;
    }
    ExecFunc(MI_AO_SetVolume(AoDevId, info->s32AoVolume), MI_SUCCESS);
    ExecFunc(MI_AO_SetMute(AoDevId, info->bAoMute), MI_SUCCESS);

    return 0;
DISABLE_ALG:

    if (info->bAoEnableVqe && bInitVqe)
    {
        ExecFunc(MI_AO_DisableVqe(AoDevId, AoChn), MI_SUCCESS);
    }

    return s32Ret;
}