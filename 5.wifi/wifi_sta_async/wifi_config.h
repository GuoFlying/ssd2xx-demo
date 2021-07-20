#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_
#include "mi_common_datatype.h"
#include "mi_wlan.h"
typedef void (*wifi_cb_t)(void *arg, void *req);

//会创建一个wifi配置线程。处理wifi阻塞的动作
int wifi_init(wifi_cb_t open_cb, wifi_cb_t state_cb);

void wifi_scan(wifi_cb_t cb, void *arg);
void wifi_connect(char *ssid, char *pwd);
void wifi_disconnect(wifi_cb_t cb, void *arg);

//在主线程中循环调用，作用：判断是否有回调函数应用，调用回调函数，才能保证是主线程调用的回调函数
void wifi_mastar_run();
#endif
