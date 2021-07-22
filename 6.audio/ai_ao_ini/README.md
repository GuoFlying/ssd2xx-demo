# audio_ini_par

### 介绍
##### 加载ini文件，设置音频默认参数
##### 分为AO、AI。设置分开。
##### 若本身无INI文件则会默认配置
##### 
``` 
编译后，生产DEMO有两个：
    load_audio_ai_tmp
    load_audio_ao_tmp
    默认生成的ini文件名为
audio_ai_par.ini
audio_ao_par.ini
```

##### 生成的demo在工程下的bin文件夹
```
拷贝至板子上，执行ai DMEO。
    ./load_audio_ai_tmp 
 运行后，开启AMIC录音。需要手动停止“ctrl+c”，关闭生成Chn0_Amic_8K_16bit_MONO.wav录音文件
    ./load_audio_ao_tmp Chn0_Amic_8K_16bit_MONO.wav
可播放，也可直接播放自己的音频文件。
若直接执行./load_audio_ao_tmp默认，播放./8K_16bit_MONO_60s.wav文件
音频文件在wav文件夹下

指定播放
    ./load_audio_ao_tmp ./wav/8K_16bit_MONO_60s.wav    
```
### API说明
##### 见 `audio_ao_par.h`和 `audio_ai_par.h`


