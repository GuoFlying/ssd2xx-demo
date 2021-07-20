#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    system("echo 11 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio11/direction");

    system("echo 10 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio10/direction");
    while (1)
    {
        system("echo 1 > /sys/class/gpio/gpio11/value");
        system("echo 0 > /sys/class/gpio/gpio10/value");
        usleep(500 * 1000);
        system("echo 0 > /sys/class/gpio/gpio11/value");
        system("echo 1 > /sys/class/gpio/gpio10/value");

        usleep(500 * 1000);
    }
    return 0;
}
