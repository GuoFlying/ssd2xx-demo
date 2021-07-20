# 概要
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
- eg:
```
#shell 命令按如下操作可控制gpio

# 控制 gpio43（TTL24）
    拉高： riu_w 0x103C 0x38 0x0011     
    拉低 ：riu_w 0x103C 0x38 0x0000
# 控制 gpio11 (SPI0_DO)
    拉高： riu_w 0x103C 0x0b 0x0011     
    拉低 ：riu_w 0x103C 0x0b 0x0000
```