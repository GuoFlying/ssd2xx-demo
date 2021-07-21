#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "pwm_cfg.h"

#define PWM1_ID 1
#define PWM0_ID 0

int main(int argc, char **argv)
{
    //PWM1(GPIO5)
    pwm_export(PWM1_ID);
    pwm_set_period(PWM1_ID,2000);
    pwm_set_duty_cycle(PWM1_ID,100);
    pwm_enable(PWM1_ID,1);

    //PWM0(GPIO4)
    pwm_export(PWM0_ID);
    pwm_set_period(PWM0_ID,2000);
    pwm_set_duty_cycle(PWM0_ID,60);
    pwm_enable(PWM0_ID,1);

    for(int i=0;i<100;i++){
        pwm_set_duty_cycle(PWM0_ID,i);
        usleep(10*1000);
    }
    pwm_set_duty_cycle(PWM0_ID,50);

    sleep(2);
    pwm_enable(PWM1_ID,0);
    pwm_enable(PWM0_ID,0);
    pwm_unexport(PWM0_ID);
    pwm_unexport(PWM1_ID);

    return 0;
}
