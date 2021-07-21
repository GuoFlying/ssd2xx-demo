#ifndef _PWM_CFG_H_
#define _PWM_CFG_H_

#include <stdint.h>

int pwm_export(const uint8_t pwm_id);
//is ： 1 使能，0 disable
int pwm_enable(const uint8_t pwm_id, const uint8_t is);

//设置PWM周期 period :单位 Hz
int pwm_set_period(const uint8_t pwm_id, const uint32_t period);

//设置PWM占空比 duty_cycle：0-100 表示0%-100%
int pwm_set_duty_cycle(const uint8_t pwm_id, const uint32_t duty_cycle);

//取消导出PWM到用户空间
int pwm_unexport(const uint8_t pwm_id);

#endif