#ifndef _UART_CFG_H_
#define _UART_CFG_H_

#include <stdint.h>
#include <termios.h>

// 串口波特率
typedef enum
{
    UART_BAUD_RATE_1200 = B1200,
    UART_BAUD_RATE_4800 = B4800,
    UART_BAUD_RATE_9600 = B9600,
    UART_BAUD_RATE_19200 = B19200,
    UART_BAUD_RATE_38400 = B38400,
    UART_BAUD_RATE_57600 = B57600,
    UART_BAUD_RATE_115200 = B115200,
    UART_BAUD_RATE_460800 = B460800,
    UART_BAUD_RATE_921600 = B921600
} UART_BAUD_E;

// 串口数据位
typedef enum
{
    UART_DATA_BIT_5 = CS5,
    UART_DATA_BIT_6 = CS6,
    UART_DATA_BIT_7 = CS7,
    UART_DATA_BIT_8 = CS8,
} UART_DATA_BIT_E;

// 串口奇偶校验位
typedef enum
{
    UART_PARITY_BIT_NONE = 'n', // 无校验
    UART_PARITY_BIT_ODD = 'o', // 奇校验
    UART_PARITY_BIT_EVEN = 'e', // 偶校验
} UART_PARITY_BIT_E;

// 串口停止位
typedef enum
{
    UART_STOP_BIT_1 = 1,
    UART_STOP_BIT_2 = 2,
} UART_STOP_BIT_E;

/**
 * @brief  打开串口
 * @param  uart_name: 输入参数, 串口名(例: /dev/ttyS1)
 * @param  baud_rate  : 输入参数, 波特率
 * @param  data_bit   : 输入参数, 数据位
 * @param  parity_bit : 输入参数, 奇偶检验位, 默认为无校验'n'或'N'
 * @param  stop_bit   : 输入参数, 停止位, 默认为1位停止位
 * @return 其他： fd
 *         失败: -1
 */
int uart_open(const char *uart_name,
              const UART_BAUD_E baud_rate,
              const UART_DATA_BIT_E data_bit,
              const UART_PARITY_BIT_E parity_bit,
              const UART_STOP_BIT_E stop_bit);

//关闭串口
int close_uart(const int fd);

//清空串口输入缓存
int uart_flush_input_data(const int fd);

//清空串口输出缓存
int uart_flush_output_data(const int fd);

//清空串口输入和输出缓存
int uart_flush_both_data(const int fd);

//串口发送数据
int uart_write_data(const int fd, const uint8_t *send_data,const uint32_t len);

//串口发送字符串
int uart_write_buff(const int fd, const char *buff);

//串口数据接收
int uart_read_data(const int fd, uint8_t *recv_data, const uint32_t len);

#endif