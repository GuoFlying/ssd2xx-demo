#include "pwm_cfg.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

// pwm路径
#define SYS_PWM_DIR "/sys/class/pwm/pwmchip0"

// 命令buf最大长度
#define CMD_BUF_MAX_LEN 100

int pwm_export(const uint8_t pwm_id)
{
    int fd = -1;
    int len = 0;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // pwm已经导出, 直接返回成功
    snprintf(cmd_buf, sizeof(cmd_buf), SYS_PWM_DIR "/pwm%d", pwm_id);
    if (0 == access(cmd_buf, F_OK))
    {
        return 0;
    }

    fd = open(SYS_PWM_DIR "/export", O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    len = snprintf(cmd_buf, sizeof(cmd_buf), "%d", pwm_id);
    if (-1 == write(fd, cmd_buf, len))
    {
        close(fd);
        return -1;
    }

    // 关闭文件
    if (-1 == close(fd))
    {
        return -1;
    }

    return 0;
}

//取消导出PWM到用户空间
int pwm_unexport(const uint8_t pwm_id)
{

    int fd = -1;
    int len = 0;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    // pwm已经导出, 直接返回成功
    snprintf(cmd_buf, sizeof(cmd_buf), SYS_PWM_DIR "/pwm%d", pwm_id);
    if (0 != access(cmd_buf, F_OK))
    {
        return 0;
    }

    fd = open(SYS_PWM_DIR "/unexport", O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    len = snprintf(cmd_buf, sizeof(cmd_buf), "%d", pwm_id);
    if (-1 == write(fd, cmd_buf, len))
    {
        close(fd);
        return -1;
    }

    // 关闭文件
    if (-1 == close(fd))
    {
        return -1;
    }

    return 0;
}

static int pwm_base_cmd(const char *buf, const uint8_t pwm_id, const uint32_t val)
{
    int fd = -1;
    int len = 0;
    char cmd_buf[CMD_BUF_MAX_LEN] = {0};

    if (!buf)
    {
        return -1;
    }

    snprintf(cmd_buf, sizeof(cmd_buf), SYS_PWM_DIR "/pwm%d/%s", pwm_id, buf);
    if (0 != access(cmd_buf, F_OK))
    {
        return -1;
    }

    fd = open(cmd_buf, O_WRONLY);
    if (fd < 0)
    {
        return -1;
    }

    len = snprintf(cmd_buf, sizeof(cmd_buf), "%d", val);
    if (-1 == write(fd, cmd_buf, len))
    {
        close(fd);
        return -1;
    }

    // 关闭文件
    if (-1 == close(fd))
    {
        return -1;
    }

    return 0;
}

int pwm_enable(const uint8_t pwm_id, const uint8_t is)
{
    return pwm_base_cmd("enable", pwm_id, is == 1 ? 1 : 0);
}

//设置PWM周期 period :单位 Hz
int pwm_set_period(const uint8_t pwm_id, const uint32_t period)
{
    return pwm_base_cmd("period", pwm_id, period);
}

//设置PWM占空比 duty_cycle：0-100 表示0%-100%
int pwm_set_duty_cycle(const uint8_t pwm_id, const uint32_t duty_cycle)
{
    return pwm_base_cmd("duty_cycle", pwm_id, duty_cycle);
}
