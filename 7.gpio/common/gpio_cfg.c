/**
 * @copyright : Copyright (c) 2020  胡恩荣
 */
#include "gpio_cfg.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

// gpio路径
#define SYS_GPIO_DIR "/sys/class/gpio"

// 命令buf最大长度
#define CMD_BUF_MAX_LEN 60

/**
 * @brief  导出gpio到用户空间
 * @param  gpio_num: 输入参数, 待导出的gpio编号
 * @return 成功: 0
 *         失败: -1
 */
int gpio_export(const uint16_t gpio_num)
{
    int ret = -1;
    int fd = -1;
    int len = 0;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // 判断gpio是否已经导出
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf),
             "%s/gpio%d", SYS_GPIO_DIR, gpio_num);
    ret = access(cmd_buf, F_OK);
    // gpio已经导出, 直接返回
    if (0 == ret)
    {
        return 0;
    }

    // 打开文件: /sys/class/gpio/export
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/export", SYS_GPIO_DIR);
    fd = open(cmd_buf, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    // export文件写入信息, 写待导出的gpio编号
    memset(cmd_buf, 0, sizeof(cmd_buf));
    len = snprintf(cmd_buf, sizeof(cmd_buf), "%d", gpio_num);
    ret = write(fd, cmd_buf, len);
    if (-1 == ret)
    {
        close(fd);

        return -1;
    }

    // 关闭文件
    ret = close(fd);
    if (-1 == ret)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief  取消导出到用户空间的gpio
 * @param  gpio_num: 输入参数, 待取消导出的gpio编号
 * @return 成功: 0
 *         失败: -1
 */
int gpio_unexport(const uint16_t gpio_num)
{
    int ret = -1;
    int fd = -1;
    int len = 0;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // 判断gpio是否导出
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/gpio%d", SYS_GPIO_DIR, gpio_num);
    ret = access(cmd_buf, F_OK);
    // gpio未导出, 直接返回
    if (-1 == ret)
    {
        return 0;
    }

    // 打开文件: /sys/class/gpio/unexport
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/unexport", SYS_GPIO_DIR);
    fd = open(cmd_buf, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    // export文件写入信息, 写待取消导出的gpio编号
    memset(cmd_buf, 0, sizeof(cmd_buf));
    len = snprintf(cmd_buf, sizeof(cmd_buf), "%d", gpio_num);
    ret = write(fd, cmd_buf, len);
    if (-1 == ret)
    {
        close(fd);

        return -1;
    }

    // 关闭文件
    ret = close(fd);
    if (-1 == ret)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief  设置gpio方向
 * @param  gpio_num : 输入参数, 待设置的gpio编号
 * @param  direction: 输入参数, 待设置的gpio方向
 * @return 成功: 0
 *         失败: -1
 */
int gpio_set_direction(const uint16_t gpio_num,
                       const enum gpio_direction direction)
{
    int fd = -1;  // 文件描述符
    int ret = -1; // 函数返回值
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // 判断gpio是否导出
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/gpio%d", SYS_GPIO_DIR, gpio_num);
    ret = access(cmd_buf, F_OK);
    // gpio未导出, 直接返回
    if (-1 == ret)
    {
        return -1;
    }

    // 打开文件: /sys/class/gpio/gpiox/direction
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf),
             "%s/gpio%d/direction", SYS_GPIO_DIR, gpio_num);
    fd = open(cmd_buf, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    switch (direction)
    {
    // 设置为输入模式
    case GPIO_IN:
    {
        ret = write(fd, "in", 3);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    // 设置为输出模式
    case GPIO_OUT:
    {
        ret = write(fd, "out", 4);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    default:
    {
        return -1;
    }
    }

    // 关闭文件
    ret = close(fd);
    if (-1 == ret)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief  设置gpio输出电平值
 * @param  gpio_num: 输入参数, 待设置的gpio编号
 * @param  value   : 输入参数, 待设置的gpio电平值
 * @return 成功: 0
 *         失败: -1
 */
int gpio_set_value(const uint16_t gpio_num, const enum gpio_value value)
{
    int ret = -1;
    int fd = -1;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // 判断gpio是否导出
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/gpio%d", SYS_GPIO_DIR, gpio_num);
    ret = access(cmd_buf, F_OK);
    // gpio未导出, 直接返回
    if (-1 == ret)
    {
        return -1;
    }

    // 打开文件: /sys/class/gpio/gpiox/value
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf),
             "%s/gpio%d/value", SYS_GPIO_DIR, gpio_num);
    fd = open(cmd_buf, O_WRONLY);
    if (fd < 0)
    {
        perror("gpio_set_value");

        return -1;
    }

    switch (value)
    {
    // 设置输出低电平
    case GPIO_LOW:
    {
        ret = write(fd, "0", 2);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    // 设置输出高电平
    case GPIO_HIGH:
    {
        ret = write(fd, "1", 2);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    default:
    {
        return -1;
    }
    }

    // 关闭文件
    ret = close(fd);
    if (-1 == ret)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief  获取gpio的电平值
 * @param  value: gpio电平值
 * @param  gpio_num: gpio编号
 * @return 成功: 0
 *         失败: -1
 */
int gpio_get_value(enum gpio_value *value, const uint16_t gpio_num)
{
    int ret = -1;
    int fd = -1;
    char ch = 0; // 获取到的gpio电平值
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // 判断gpio是否导出
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/gpio%d", SYS_GPIO_DIR, gpio_num);
    ret = access(cmd_buf, F_OK);
    // gpio未导出, 直接返回
    if (-1 == ret)
    {
        return -1;
    }

    // 打开文件: /sys/class/gpio/gpiox/value
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf),
             "%s/gpio%d/value", SYS_GPIO_DIR, gpio_num);
    fd = open(cmd_buf, O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }

    // 获取gpio电平值
    ret = read(fd, &ch, 1);
    if (-1 == ret)
    {
        close(fd);

        return -1;
    }

    // 根据获取到的电平值, 返回数据
    switch (ch)
    {
    case '0':
    {
        *value = GPIO_LOW;

        break;
    }

    case '1':
    {
        *value = GPIO_HIGH;

        break;
    }

    default:
    {
        close(fd);

        return -1;
    }
    }

    // 关闭文件
    ret = close(fd);
    if (-1 == ret)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief  设置gpio触发边沿
 * @param  gpio_num: 输入参数, 待设置的gpio编号
 * @param  edge    : 输入参数, 待设置的gpio边沿
 * @return 成功: 0
 *         失败: -1
 */
int gpio_set_edge(const uint16_t gpio_num, const enum gpio_edge edge)
{
    int ret = -1;
    int fd = -1;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // 判断gpio是否导出
    memset(cmd_buf, 0, sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s/gpio%d", SYS_GPIO_DIR, gpio_num);
    ret = access(cmd_buf, F_OK);
    // gpio未导出, 直接返回
    if (-1 == ret)
    {
        return -1;
    }

    // 打开文件: /sys/class/gpio/gpiox/edge
    memset(cmd_buf, 0, sizeof(cmd_buf));
    ret = snprintf(cmd_buf, sizeof(cmd_buf),
                   "%s/gpio%d/edge", SYS_GPIO_DIR, gpio_num);
    fd = open(cmd_buf, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    switch (edge)
    {
    // 不是中断
    case GPIO_NONE:
    {
        ret = write(fd, "none", strlen("none") + 1);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    // 中断输入, 上升沿
    case GPIO_RISING:
    {
        ret = write(fd, "rising", strlen("rising") + 1);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    // 中断输入, 下降沿
    case GPIO_FALLING:
    {
        ret = write(fd, "falling", strlen("falling") + 1);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    // 中断输入, 上升沿和下降沿
    case GPIO_BOTH:
    {
        ret = write(fd, "both", strlen("both") + 1);
        if (-1 == ret)
        {
            close(fd);

            return -1;
        }

        break;
    }

    default:
    {
        return -1;
    }
    }

    // 关闭文件
    ret = close(fd);
    if (-1 == ret)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief  打开gpio设备
 * @param  gpio_num: 输入参数, 待打开的gpio编号
 * @return 成功: 返回设备文件描述符
 *         失败: -1
 */
int gpio_open(const uint16_t gpio_num)
{
    int fd = -1;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    snprintf(cmd_buf, sizeof(cmd_buf),
             "%s/gpio%d/value", SYS_GPIO_DIR, gpio_num);

    fd = open(cmd_buf, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        return -1;
    }

    return fd;
}

/**
 * @brief  关闭gpio设备
 * @param  fd: 输入参数, 待关闭的设备文件描述符
 * @return 成功: 0
 *         失败: -1
 */
int gpio_close(const int fd)
{
    int ret = -1;

    ret = close(fd);

    return ret;
}