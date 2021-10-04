# 概要

- 音频相关 DMEO

## ai_ao_ini

- 若不存在ini文件则会默认创建，之后运行demo，会自动加载ini内部的参数来设置AI、AO
- SSD202 - v030版本的SDK验证
- 主要使用MI的AI、AO的接口
- **运行demo时，会在运行路径下创建ini文件，需要确保当前运行的文件夹有写的权限。否则可能运行失败**
- [详细说明](./ai_ao_ini/README.md)；

## mp3_player

- 原厂简单MP3播放 <https://gitee.com/mFlying/RunTimeDemo/tree/master/MultiPlayer/mp3player>
- 运行

```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
./Mp3Player layout_test.mp3
```

## alsa_playback_1

- 简单调用alsa playback 播放音频测试
- 环境中需要存在alsa lib以及相关配置

```shell
./alsa_playback_1 ./res/48K_16bit_STERO_30s.wav
```

## alsa_playback_2

- 简单调用alsa playback以及mixer 播放音频测试
- 环境中需要存在alsa lib以及相关配置
- 可调整音量

```shell
./alsa_playback_2 ./res/48K_16bit_STERO_30s.wav 20000
```
