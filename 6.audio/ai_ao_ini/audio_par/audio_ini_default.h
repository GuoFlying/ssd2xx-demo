/*********************************************************************************
  *FileName:  audio_ini_default.h
  *Author:  flying
  *Version:  v1.0
  *Date:    2021-05-23
**********************************************************************************/
#ifndef _AUDIO_DEFAULT_H_
#define _AUDIO_DEFAULT_H_
/*AI
其他参数AI文档
*/

#define INI_AI_PARAMETER                                              \
    "#\n"                                                             \
    "# This is an audio AI default parameter of ini file\n"           \
    "#\n"                                                             \
    "\n"                                                              \
    "[AI]\n"                                                          \
    "bAiEnableAed           = false ;\n"                              \
    "bAiEnableSsl           = false ;\n"                              \
    "bAiEnableBf            = false ;\n"                              \
    "bAiSetBfDoa            = false ;\n"                              \
    "s32AiBfDoa             = 0 ;\n"                                  \
    "bAiEnableVqe           = true ;\n"                               \
    "bAiEnableResample      = false ;\n"                              \
    "bAiEnableAenc          = false ;\n"                              \
    "\n"                                                              \
    "[AED]\n"                                                         \
    "bEnableNr              = true ;\n"                               \
    "eSensitivity           = E_MI_AUDIO_AED_SEN_HIGH ;\n"            \
    "s32OperatingPoint      = -5 ;\n"                                 \
    "s32VadThresholdDb      = -40 ;\n"                                \
    "s32LsdThresholdDb      = -15 ;\n"                                \
    "\n"                                                              \
    "[SSL]\n"                                                         \
    "bBfMode                = false ;\n"                              \
    "s32Temperature         = 25 ;\n"                                 \
    "s32NoiseGateDbfs       = -40 ;\n"                                \
    "s32DirectionFrameNum   = 300 ;\n"                                \
    "\n"                                                              \
    "[BF]\n"                                                          \
    "u32MicDistance         = 0 ;\n"                                  \
    "u32ChanCnt             = 2 ;\n"                                  \
    "s32Temperature         = 25 ;\n"                                 \
    "s32NoiseGateDbfs       = -40 ;\n"                                \
    "s32NoiseSupressionMode = 8 ;\n"                                  \
    "s32NoiseEstimation     = 1 ;\n"                                  \
    "outputGain             = 0.7 ;\n"                                \
    "\n"                                                              \
    "[VQE]\n"                                                         \
    "u32ChnNum              = 1 ;\n"                                  \
    "bAecOpen               = true ;\n"                               \
    "bAgcOpen               = true ;\n"                               \
    "bAnrOpen               = true ;\n"                               \
    "bEqOpen                = true ;\n"                               \
    "bHpfOpen               = true ;\n"                               \
    "s32FrameSample         = 128 ;\n"                                \
    "s32WorkSampleRate      = E_MI_AUDIO_SAMPLE_RATE_8000 ;\n"        \
    "AoDevId                = 0 ;\n"                                  \
    "AoChn                  = 0 ;\n"                                  \
    "\n"                                                              \
    "[AEC]\n"                                                         \
    "\n"                                                              \
    "bComfortNoiseEnable    = false ;\n"                              \
    "s16DelaySample         = 0 ;\n"                                  \
    "u32AecSupfreq          = {4, 6, 36, 49, 50, 51} ;\n"             \
    "u32AecSupIntensity     = {5, 4, 4, 5, 10, 10, 10} ;\n"           \
    "\n"                                                              \
    "[HPF]\n"                                                         \
    "\n"                                                              \
    "eMode                  = E_MI_AUDIO_ALGORITHM_MODE_USER ;\n"     \
    "eHpfFreq               = E_MI_AUDIO_HPF_FREQ_150 ;\n"            \
    "\n"                                                              \
    "[ANR]\n"                                                         \
    "\n"                                                              \
    "eMode                  = E_MI_AUDIO_ALGORITHM_MODE_MUSIC ;\n"    \
    "u32NrIntensity         = 15 ;\n"                                 \
    "u32NrSmoothLevel       = 10 ;\n"                                 \
    "eNrSpeed               = E_MI_AUDIO_NR_SPEED_MID ;\n"            \
    "\n"                                                              \
    "[AGC]\n"                                                         \
    "\n"                                                              \
    "eMode                  = E_MI_AUDIO_ALGORITHM_MODE_USER ;\n"     \
    "s32NoiseGateDb         = -60 ;\n"                                \
    "s32TargetLevelDb       = -3 ;\n"                                 \
    "stAgcGainInfo.s32GainInit      = 0 ;\n"                          \
    "stAgcGainInfo.s32GainMax       = 20 ;\n"                         \
    "stAgcGainInfo.s32GainMin       = 0 ;\n"                          \
    "u32AttackTime                  = 1 ;\n"                          \
    "s16Compression_ratio_input     = {-80, -60, -40, -25, 0} ;\n"    \
    "s16Compression_ratio_output    = {-80, -30, -15, -10, -3} ;\n"   \
    "u32DropGainMax                 = 12 ;\n"                         \
    "u32NoiseGateAttenuationDb      = 0 ;\n"                          \
    "u32ReleaseTime                 = 3 ;\n"                          \
    "\n"                                                              \
    "[EQ]\n"                                                          \
    "\n"                                                              \
    "eMode                  = E_MI_AUDIO_ALGORITHM_MODE_USER ;\n"     \
    "s16EqGainDb            = {"                                      \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                            \
    "3,3,3,3,3,3,3,3"                                                 \
    "} ;\n"                                                           \
    "\n"                                                              \
    "[RATE]\n"                                                        \
    "eAiOutputResampleRate      = E_MI_AUDIO_SAMPLE_RATE_INVALID ;\n" \
    "\n"                                                              \
    "[AENC]\n"                                                        \
    "eAencType                  = E_MI_AUDIO_AENC_TYPE_INVALID ;\n"   \
    "cfg.eSamplerate            = E_MI_AUDIO_SAMPLE_RATE_8000 ;\n"    \
    "cfg.eSoundmode             = E_MI_AUDIO_SOUND_MODE_MONO ;\n"     \
    "cfg.eG726Mode              = E_MI_AUDIO_G726_MODE_16 ;\n"        \
    "\n"

/*AO
bAoMute 是否静音
s32AoVolume 通道的音量大小
bAoEnableVqe	是否使能
其他参数AO文档
*/
#define INI_AO_PARAMETER                                            \
    "#\n"                                                           \
    "# This is an audio AO default parameter of ini file\n"         \
    "#\n"                                                           \
    "\n"                                                            \
    "[AO]\n"                                                        \
    "\n"                                                            \
    "s32AoVolume        = -5 ;\n"                                   \
    "bAoMute            = false ;\n"                                \
    "bAoEnableVqe       = true ;\n"                                 \
    "\n"                                                            \
    "[VQE]\n"                                                       \
    "\n"                                                            \
    "bAgcOpen           = true ;\n"                                 \
    "bAnrOpen           = true ;\n"                                 \
    "bEqOpen            = true ;\n"                                 \
    "bHpfOpen           = false ;\n"                                \
    "s32FrameSample     = 128 ;\n"                                  \
    "s32WorkSampleRate  = E_MI_AUDIO_SAMPLE_RATE_8000 ;\n"          \
    "\n"                                                            \
    "[HPF]\n"                                                       \
    "\n"                                                            \
    "eMode              = E_MI_AUDIO_ALGORITHM_MODE_USER ;\n"       \
    "eHpfFreq           = E_MI_AUDIO_HPF_FREQ_150 ;\n"              \
    "\n"                                                            \
    "[ANR]\n"                                                       \
    "\n"                                                            \
    "eMode              = E_MI_AUDIO_ALGORITHM_MODE_MUSIC ;\n"      \
    "u32NrIntensity     = 15 ;\n"                                   \
    "u32NrSmoothLevel   = 10 ;\n"                                   \
    "eNrSpeed           = E_MI_AUDIO_NR_SPEED_MID ;\n"              \
    "\n"                                                            \
    "[AGC]\n"                                                       \
    "\n"                                                            \
    "eMode              = E_MI_AUDIO_ALGORITHM_MODE_USER ;\n"       \
    "s32NoiseGateDb     = -60 ;\n"                                  \
    "s32TargetLevelDb   = -3 ;\n"                                   \
    "stAgcGainInfo.s32GainInit      = 0 ;\n"                        \
    "stAgcGainInfo.s32GainMax       = 20 ;\n"                       \
    "stAgcGainInfo.s32GainMin       = 0 ;\n"                        \
    "u32AttackTime                  = 1 ;\n"                        \
    "s16Compression_ratio_input     = {-80, -60, -40, -25, 0} ;\n"  \
    "s16Compression_ratio_output    = {-80, -30, -15, -10, -3} ;\n" \
    "u32DropGainMax                 = 12 ;\n"                       \
    "u32NoiseGateAttenuationDb      = 0 ;\n"                        \
    "u32ReleaseTime                 = 3 ;\n"                        \
    "\n"                                                            \
    "[EQ]\n"                                                        \
    "\n"                                                            \
    "eMode          = E_MI_AUDIO_ALGORITHM_MODE_USER ;\n"           \
    "s16EqGainDb    = {"                                            \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3,3,3,"                                          \
    "3,3,3,3,3,3,3,3"                                               \
    "} ;\n"                                                         \
    "\n"

#endif