#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "gpio_cfg.h"

int main(int argc, char **argv)
{
    gpio_export(10);
    gpio_export(11);
    gpio_set_direction(10, GPIO_OUT);
    gpio_set_direction(11, GPIO_OUT);
    while (1)
    {
        gpio_set_value(10, GPIO_HIGH);
        gpio_set_value(11, GPIO_LOW);
        usleep(500 * 1000);
        gpio_set_value(11, GPIO_HIGH);
        gpio_set_value(10, GPIO_LOW);
        usleep(500 * 1000);
    }
    return 0;
}
