/*********************************************************************************
  *FileName:  audio_ao_par_common.h
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-23
**********************************************************************************/
#ifndef _AUDIO_AO_PAR_COMMON_H_
#define _AUDIO_AO_PAR_COMMON_H_

#include "mi_common_datatype.h"
#include <stdio.h>
#include <string.h>

#define P_NONE "\e[0m"
#define P_BLACK "\e[0;30m"
#define P_RED "\e[0;31m"
#define P_GREEN "\e[0;32m"

#define ExecFunc(func, _ret_)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        MI_S32 s32TmpRet;                                                                                              \
        printf("\n%d Start test: %s\n", __LINE__, #func);                                                              \
        s32TmpRet = func;                                                                                              \
        if (s32TmpRet != _ret_)                                                                                        \
        {                                                                                                              \
            printf(P_RED "AUDIO_TEST [%d] %s exec function failed, result:0x%x\n" P_NONE, __LINE__, #func, s32TmpRet); \
            return 1;                                                                                                  \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            printf("AUDIO_TEST [%d] %s  exec function pass\n", __LINE__, #func);                                       \
        }                                                                                                              \
        printf("%d End test: %s\n", __LINE__, #func);                                                                  \
    } while (0);

#define ExecFuncNoExit(func, _ret_, __ret)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        MI_S32 s32TmpRet;                                                                                              \
        printf("\n%d Start test: %s\n", __LINE__, #func);                                                              \
        s32TmpRet = func;                                                                                              \
        __ret = s32TmpRet;                                                                                             \
        if (s32TmpRet != _ret_)                                                                                        \
        {                                                                                                              \
            printf(P_RED "AUDIO_TEST [%d] %s exec function failed, result:0x%x\n" P_NONE, __LINE__, #func, s32TmpRet); \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            printf("AUDIO_TEST [%d] %s  exec function pass\n", __LINE__, #func);                                       \
        }                                                                                                              \
        printf("%d End test: %s\n", __LINE__, #func);                                                                  \
    } while (0);

#endif