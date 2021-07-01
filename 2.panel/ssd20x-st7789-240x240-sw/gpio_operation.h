#ifndef __GPIO_OPERATION__H__
#define __GPIO_OPERATION__H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void _user_set_gpio_value(int gpio_num, unsigned char value);
void _user_export_gpio(int gpio_num);
void _user_set_gpio_direction(int gpio_num, char *pdirection);


#endif
