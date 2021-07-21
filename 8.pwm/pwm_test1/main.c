#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    //PWM1(GPIO5)
    system("echo 1 > /sys/class/pwm/pwmchip0/export");
    system("echo 2000 > /sys/class/pwm/pwmchip0/pwm1/period"); 
    system("echo 100 > /sys/class/pwm/pwmchip0/pwm1/duty_cycle");   //占空比100%
    system("echo 1 > /sys/class/pwm/pwmchip0/pwm1/enable"); 

    //PWM0(GPIO4)
    system("echo 0 > /sys/class/pwm/pwmchip0/export");
    system("echo 2000 > /sys/class/pwm/pwmchip0/pwm0/period"); //频率2kHz
    system("echo 60 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");   //占空比30%
    system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable"); //使能
    for(int i=0;i<100;i++){
        char buff[100];
        sprintf(buff,"echo %d > /sys/class/pwm/pwmchip0/pwm0/duty_cycle",i);
        system(buff);
        usleep(10*1000);
    }
    system("echo 50 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle"); 

    sleep(2);
     system("echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable"); //取消使能
     system("echo 0 > /sys/class/pwm/pwmchip0/pwm1/enable"); 
     system("echo 1 > /sys/class/pwm/pwmchip0/unexport"); //导出
     system("echo 0 > /sys/class/pwm/pwmchip0/unexport");
    return 0;
}
