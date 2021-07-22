# 概要
- 参考SDK UART文档。
- SSD20x引脚
```shell
# 公版demo板四路uart目前对应关系如下：
 
make infinity2m_spinand_ssc011a_s01a_minigui_defconfig
vim arch/arm/boot/dts/infinity2m-ssc011a-s01a-padmux-display.dtsi

<PAD_FUART_RX       PINMUX_FOR_FUART_MODE_2     MDRV_PUSE_FUART_RX >,
<PAD_FUART_TX       PINMUX_FOR_FUART_MODE_2     MDRV_PUSE_FUART_TX >,
<PAD_UART1_RX       PINMUX_FOR_UART1_MODE_1     MDRV_PUSE_UART1_RX >,
<PAD_UART1_TX       PINMUX_FOR_UART1_MODE_1     MDRV_PUSE_UART1_TX >,
<PAD_GPIO8          PINMUX_FOR_UART2_MODE_2     MDRV_PUSE_UART2_RX >,
<PAD_GPIO9           PINMUX_FOR_UART2_MODE_2     MDRV_PUSE_UART2_TX >,
# PM_UART是终端串口，默认已经配置好，不需要额外配置
< PAD_PM_UART_RX          PINMUX_FOR_UART0_MODE_0     MDRV_PUSE_UART0_RX >,
< PAD_PM_UART_TX          PINMUX_FOR_UART0_MODE_0     MDRV_PUSE_UART0_TX >,
```

- SSD2X2串口引脚

|  UART  | Group | Mode | TX | RX | CT | RTS | DEV  |
|  ----  | ----  |  ----  | ----  |  ----  | ----  |  ----  | ----  |
| Fuart  | 6  | PAD_GPIO5  | PAD_GPIO6  | PAD_GPIO7  | PAD_GPIO8  | /dev/ttyS2 | 
| Uart1 |	8	|PAD_GPIO1|	PAD_GPIO2	|--	|--|	/dev/ttyS1|
| Uart2 |	6	|PAD_GPIO3| PAD_GPIO4 |	--	|--|	/dev/ttyS3|

- kernel 中驱动、设备树需要对应的配置

### common
- 通用uart接口

### uart_test1
- 简单调用system接口控制pwm
- uart简单发送接收

### uart_test2
- 调用了封装的uart接口实现,实现发送接收
- 采用轮询的方式接收数据，会占用大量的cpu，可挂后台，之后使用`top`命令查看cpu占用

### uart_test3_poll
- 调用了封装的uart接口实现,实现发送接收
- 开启了一个接收线程，专门接收串口数据
- 使用了poll IO复用，可保证数据及时收取，并有效降低CPU占用。
- 接收到最后一个字符为'\n'时 || 数据超过1023个字节 || 500ms没接收到其他数据时，打印接收到是数据

### uart_test4_signal
- 调用了封装的uart接口实现,实现发送接收
- 中断的方式接收串口数据
- 若没收到数据时，线程永久休眠。当有数据时，会触发中断，停止休眠