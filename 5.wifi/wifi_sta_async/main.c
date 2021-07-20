#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mi_common_datatype.h"
#include "mi_wlan.h"
#include "wifi_config.h"

static void wifi_scan_cb(void *arg, void *req)
{

    printf("%s %d : %s\r\n", __func__, __LINE__);

    MI_WLAN_ScanResult_t *scanResult = req;
    if (!scanResult)
    {
        return;
    }

    printf("Scan result:\n");
    if (scanResult->u8APNumber <= 0)
    {
        printf("第一次可能会获取失败，再次获取试下\n");
    }

    //排序
    for (int i = 0; i < scanResult->u8APNumber - 1; i++)
    {
        for (int j = 0; j < scanResult->u8APNumber - i - 1; j++)
        {
            if (scanResult->stAPInfo[j].stQuality.signalSTR < scanResult->stAPInfo[j + 1].stQuality.signalSTR)
            {
                MI_WLAN_APInfo_t tmp;
                memcpy(&tmp, &scanResult->stAPInfo[j], sizeof(MI_WLAN_APInfo_t));
                memcpy(&scanResult->stAPInfo[j], &scanResult->stAPInfo[j + 1], sizeof(MI_WLAN_APInfo_t));
                memcpy(&scanResult->stAPInfo[j + 1], &tmp, sizeof(MI_WLAN_APInfo_t));
            }
        }
    }

    for (int i = 0; i < scanResult->u8APNumber; i++)
    {
        char *pSsid = (char *)scanResult->stAPInfo[i].au8SSId;
        if (pSsid && strcmp(pSsid, "\"\""))
        {
            // printf("SSID: %s\n", pSsid);
            // printf("MAC: %s\n", (char *)scanResult->stAPInfo[i].au8Mac);
            // printf("encrypt: %s\n", scanResult->stAPInfo[i].bEncryptKey ? "true" : "false");
            // printf("signalSTR: %d db\n", scanResult->stAPInfo[i].stQuality.signalSTR);
            // printf("frequency: %f GHz\n", scanResult->stAPInfo[i].fFrequency);
            // printf("bitrate: %f Mb/s\n", scanResult->stAPInfo[i].fBitRate);
            // printf("channel: %d\n", scanResult->stAPInfo[i].u8Channel);
            // printf("channel: %d\n", scanResult->stAPInfo[i].u16CellId);
            // printf("\n");
            printf(" [%s] [%d] [%d] [%d]\n", pSsid, scanResult->stAPInfo[i].bEncryptKey, scanResult->stAPInfo[i].stQuality.signalSTR, scanResult->stAPInfo[i].eEncrypt);
        }
    }

    free(scanResult);
}

static void wifi_demo_cmd_help()
{
    printf("please input option command:\n");
    printf("g. 扫描wifi\n");
    printf("c. 连接wifi,之后按提示输入\n");
    printf("d. 手动断开wifi连接\n");
}

static void wifi_open_cb(void *arg, void *req)
{
    printf("%s %d\r\n", __func__, __LINE__);
    int is = (int)req;
    if (is)
    {
        printf("wifo open success\r\n");
    }
    else
    {
        printf("wifo open error\r\n");
    }
    wifi_demo_cmd_help();
}

static void wifi_state_cb(void *arg, void *req)
{
    printf("%s %d\r\n", __func__, __LINE__);
    MI_WLAN_Status_sta_t *st = req;
    if (!st)
    {
        return;
    }
    if (WPA_COMPLETED == st->state)
    {
        printf("连接成功： [%s] [%s] [%d] [%d]\n", st->ssid, st->address, st->channel, st->RSSI);
    }
    else if (WPA_DISCONNECTED == st->state)
    {
        printf("断开连接 [%s] [%s] [%d] [%d]\n", st->ssid, st->address, st->channel, st->RSSI);
    }
}

static void wifi_disconnect_cb(void *arg, void *req)
{
    printf("手动断开wifi %s %d\r\n", __func__, __LINE__);
}

static __inline int kbhit(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    /* Wait up to five seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)
    {
        perror("select()");
        return 0;
    }
    else if (retval)
        return 1;
    /* FD_ISSET(0, &rfds) will be true. */
    else
        return 0;
    return 0;
}

int main(int argc, char **argv)
{

    wifi_init(wifi_open_cb, wifi_state_cb);

    char ssid[64];
    char pwd[64];
    while (1)
    {
        //非阻塞方式
        if (kbhit())
        {
            const int key = getchar();
            switch (key)
            {
            case 'g':
                wifi_scan(wifi_scan_cb, NULL);
                break;
            case 'c':
                printf("input SSID:");
                scanf("%s", ssid);
                printf("input pwd:");
                scanf("%s", pwd);
                printf("connect ing... %s %s\r\n", ssid, pwd);
                wifi_connect(ssid, pwd);
                break;
            case 'd':
                wifi_disconnect(wifi_disconnect_cb, NULL);
                break;
            default:
                printf("~/\r\n");
                break;
            }
        }

        wifi_mastar_run(); //循环判断有没有wifi回调，有的话会自动调用回调
        usleep(50 * 1000);
    }

    MI_WLAN_DeInit();
    return 0;
}
