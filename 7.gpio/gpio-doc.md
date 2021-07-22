# 概要
```
1、dtsi把对应的引脚配置成普通IO模式
如:
<PAD_PM_LED0            PINMUX_FOR_GPIO_MODE      MDRV_PUSE_NA>,
<PAD_PM_LED1            PINMUX_FOR_GPIO_MODE      MDRV_PUSE_NA>,

2、之后进入/sys/class/gpio中可直接操作文件。
如下概要：
direction 
    设置输出还是输入模式 
    设置为输入：echo “in” > direction
    设置为输出：echo “out” > direction
value 
    输出时，控制高低电平；输入时，获取高低电平 
    高电平：echo 1 > value
    低电平：echo 0 > value
edge 
    控制中断触发模式，引脚被配置为中断后可以使用poll() 函数监听引脚 
    非中断引脚： echo “none” > edge
    上升沿触发：echo “rising” > edge
    下降沿触发：echo “falling” > edge
    边沿触发：echo “both” > edge

3、目前只支持上升沿或则下降沿触发。不支持边沿触发。

4、用户态下即可使用poll监听

```
### common
- 通用gpio接口

### gpio_test1
- 简单调用system接口控制gpio输出高低电平


### gpio_test2
- 调用封装的通用接口控制gpio输出高低电平

### gpio_test_input
- 调用封装的通用接口，通过中断的方式检测按键输入。
- 中断 不支持边缘触发方式。。平台驱动问题


### gpio_test_fast_rollover
- SSD20x V030 上验证测试
- 直接调用寄存器实现快速翻转 
- 寄存器操作参考`kernel/drivers/sstar/samples/riu.c`
- 寄存器查看数据手册
- 通过操作寄存器的方式，翻转速度可达21.6MHz
- 关键：bit5：（0：output ，1：intput）bit4（0：输出低电平，1：输出高电平）bit0：（当前IO口读取到的电平数值）
- eg:
```
#shell 命令按如下操作可控制gpio

# 控制 gpio43（TTL24）
    拉高： riu_w 0x103C 0x38 0x0010     
    拉低 ：riu_w 0x103C 0x38 0x0000
# 控制 gpio11 (SPI0_DO)
    拉高： riu_w 0x103C 0x0b 0x0010     
    拉低 ：riu_w 0x103C 0x0b 0x0000
```