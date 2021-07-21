# 概要
- 参考SDK pwm开发文档，需要对对应的设备树进行配置，以及需要开启kernel pwm配置选项。

- SSD20X配置快速参考
```shell
    # kernel 
    make infinity2m_spinand_ssc011a_s01a_minigui_defconfig
    # 打开pwm配置 （arm 4.9.84 Kernel Configuration → Device Drivers → SStar SoC platform drivers → <*>  SSTAR_PWM）
    make menuconfig 
    # 修改设备树
    vim arch/arm/boot/dts/infinity2m-ssc011a-s01a-display.dtsi
    #~~~~~
        pwm {
            compatible = "sstar,infinity-pwm";
            reg = <0x1F003400 0x600>;
            clocks = <&CLK_xtali_12m>;
            npwm = <2>;
            pad-ctrl = <PAD_GPIO4 PAD_GPIO5>;
            status = "ok";  // no available pads
        };  
    #~~~~~
    # 修改设备树（需要把关联的引脚屏蔽）
    vim arch/arm/boot/dts/infinity2m-ssc011a-s01a-padmux-display.dtsi
    #~~~~~
        <PAD_GPIO4          PINMUX_FOR_PWM0_MODE_3      MDRV_PUSE_PWM0 >,
        <PAD_GPIO5          PINMUX_FOR_PWM1_MODE_4      MDRV_PUSE_PWM1 >,
        //<PAD_GPIO6          PINMUX_FOR_EJ_MODE_3        MDRV_PUSE_EJ_TDO >,
        //<PAD_GPIO7          PINMUX_FOR_EJ_MODE_3        MDRV_PUSE_EJ_TDI >,
    #~~~~~
```
### common
- 通用pwm接口

### pwm_test1
- 简单调用system接口控制pwm
- SSD202原厂默认屏幕背光使能为GPIO5.（高电平有效）。背光调节pwm为PWM4.

### pwm_test2
- 调用了封装的pwm接口实现了与test1一样的功能
