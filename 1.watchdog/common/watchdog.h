#ifndef _WATCHDOG_H__
#define _WATCHDOG_H__

#define WATCHDOG_DEV_NAME "/dev/watchdog"

//返回fd
int watchdog_open();
void watchdog_start(int fd, int second);
void watchdog_feed(int fd);
void watchdog_stop(int fd);
void watchdog_close(int fd);

#endif