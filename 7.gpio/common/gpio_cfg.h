/**
 * @copyright : Copyright (c) 2020  胡恩荣
 */
#ifndef _GPIO_CFG_H_
#define _GPIO_CFG_H_

#include <stdint.h>

// gpio方向
enum gpio_direction
{
    GPIO_IN = 0,
    GPIO_OUT = 1
};

// gpio电平值
enum gpio_value
{
    GPIO_LOW = 0,
    GPIO_HIGH = 1
};

// gpio中断输入, 边沿值
enum gpio_edge
{
    GPIO_NONE = 0,    // 不是中断
    GPIO_RISING = 1,  // 中断输入, 上升沿
    GPIO_FALLING = 2, // 中断输入, 下降沿
    GPIO_BOTH = 3     // 中断输入, 上升沿和下降沿
};

/**
 * @brief  导出gpio到用户空间
 * @param  gpio_num: 输入参数, 待导出的gpio编号
 * @return 成功: 0
 *         失败: -1
 */
int gpio_export(const uint16_t gpio_num);

/**
 * @brief  取消导出到用户空间的gpio
 * @param  gpio_num: 输入参数, 待取消导出的gpio编号
 * @return 成功: 0
 *         失败: -1
 */
int gpio_unexport(const uint16_t gpio_num);

/**
 * @brief  设置gpio方向
 * @param  gpio_num : 输入参数, 待设置的gpio编号
 * @param  direction: 输入参数, 待设置的gpio方向
 * @return 成功: 0
 *         失败: -1
 */
int gpio_set_direction(const uint16_t gpio_num,
                       const enum gpio_direction direction);

/**
 * @brief  设置gpio输出电平值
 * @param  gpio_num: 输入参数, 待设置的gpio编号
 * @param  value   : 输入参数, 待设置的gpio电平值
 * @return 成功: 0
 *         失败: -1
 */
int gpio_set_value(const uint16_t gpio_num, const enum gpio_value value);

/**
 * @brief  获取gpio的电平值
 * @param  value: gpio电平值
 * @param  gpio_num: gpio编号
 * @return 成功: 0
 *         失败: -1
 */
int gpio_get_value(enum gpio_value *value, const uint16_t gpio_num);

/**
 * @brief  设置gpio触发边沿
 * @param  gpio_num: 输入参数, 待设置的gpio编号
 * @param  edge    : 输入参数, 待设置的gpio边沿
 * @return 成功: 0
 *         失败: -1
 */
int gpio_set_edge(const uint16_t gpio_num, const enum gpio_edge edge);

/**
 * @brief  打开gpio设备
 * @param  gpio_num: 输入参数, 待打开的gpio编号
 * @return 成功: 返回设备文件描述符
 *         失败: -1
 */
int gpio_open(const uint16_t gpio_num);

/**
 * @brief  关闭gpio设备
 * @param  fd: 输入参数, 待关闭的设备文件描述符
 * @return 成功: 0
 *         失败: -1
 */
int gpio_close(const int fd);
#endif