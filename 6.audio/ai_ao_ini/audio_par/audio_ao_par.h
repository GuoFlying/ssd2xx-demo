/*********************************************************************************
  *FileName:  audio_ao_par.h
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-23
**********************************************************************************/
#ifndef _AUDIO_AO_PAR_H_
#define _AUDIO_AO_PAR_H_
#include "audio_par_common.h"
#include "iniparser.h"
#include "audio_ini_default.h"
#include "mi_ao.h"


/*参数见API文档说明*/
typedef struct audio_ao_par_info_t_
{
    MI_AUDIO_DEV AoDevId;
    MI_AO_CHN AoChn;

    MI_BOOL bAoEnableVqe;
    MI_AO_VqeConfig_t stAoSetVqeConfig;

    MI_S32 s32AoVolume;
    MI_BOOL bAoMute;

} audio_ao_par_info_t;

/*-------------------------------------------------------------------------*/
/**
int audio_ao_par_analysis(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, const char *ini_path);
 * 
  @brief    ao 参数从INI中获取
  @param    AoDevId     AO设备ID
  @param    AoChn       AO通道号.
  @param    ini_path    ini 配置文件地址以及文件名，如"./audio_ai_par.ini"
  @return  0：成功 other：失败

    调用该接口后，该模块会记录参数信息，若要释放资源必须调用audio_ao_par_close
 */
/*--------------------------------------------------------------------------*/
int audio_ao_par_analysis(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, const char *ini_path);
/*-------------------------------------------------------------------------*/
/**
int audio_ao_par_execute(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
 * 
  @brief    执行AO参数设置
  @param    AoDevId     AO设备ID
  @param    AoChn       AO通道号.
  @return  0：成功 other：失败

    支持的参数为audio_ao_par_analysis获取到的参数
 */
/*--------------------------------------------------------------------------*/
int audio_ao_par_execute(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
/*-------------------------------------------------------------------------*/
/**
audio_ao_par_info_t *audio_ao_get_par(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
 * 
  @brief    获取AO参数信息
  @param    AoDevId     AO设备ID
  @param    AoChn       AO通道号.
  @return  NULL：失败 other：audio_ao_par_info_t 信息

    获取参数信息
 */
/*--------------------------------------------------------------------------*/
audio_ao_par_info_t *audio_ao_get_par(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
/*-------------------------------------------------------------------------*/
/**
int audio_ao_par_close(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
 * 
  @brief    关闭AO相关设置，释放子资源
  @param    AoDevId     AO设备ID
  @param    AoChn       AO通道号.
  @return  0：成功 other：失败

 */
/*--------------------------------------------------------------------------*/
int audio_ao_par_close(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
#endif
