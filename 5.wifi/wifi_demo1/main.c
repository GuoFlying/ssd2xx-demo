#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mi_common_datatype.h"
#include "mi_wlan.h"

static char *g_ssid = NULL;
static char *g_pwd = NULL;
static MI_WLAN_OpenParams_t g_stOpenParam = {E_MI_WLAN_NETWORKTYPE_INFRA};

typedef struct wifi_exe_t_
{
    char cmd;
    void (*func)(void);
} wifi_exe_t;

void wifi_demo_cmd_help()
{
    printf("please input option command:\n");
    printf("m. switch STA/AP mode, input 'm'\n");
    printf("c. connect wifi hotspot, input 'n'\n");
    printf("d. disconnect wifi hotspot, input 'd'\n");
    printf("p. print wifi hotspot's info, input 'p'\n");
    printf("h. help\n");
    printf("q. exit, input 'q'\n\n");
}

void wifi_exe_switch_mode()
{
    if (g_stOpenParam.eNetworkType == E_MI_WLAN_NETWORKTYPE_INFRA)
    {
        g_stOpenParam.eNetworkType = E_MI_WLAN_NETWORKTYPE_AP;
        printf("wlan mode: AP\n");
    }
    else
    {
        g_stOpenParam.eNetworkType = E_MI_WLAN_NETWORKTYPE_INFRA;
        printf("wlan mode: STA\n");
    }
}

static WLAN_HANDLE g_wlanHdl = -1;
static int g_is_connect = 0;

void wifi_exe_connect()
{
    MI_WLAN_ConnectParam_t ConnectParam;
    if (g_is_connect == 1)
    {
        printf("已经连接成功,需要先断开\n");
        return;
    }

    if (MI_WLAN_Open(&g_stOpenParam))
    {
        printf("wlan open failed\n");
        return;
    }

    memset(&ConnectParam, 0, sizeof(ConnectParam));
    ConnectParam.OverTimeMs = 5000;
    ConnectParam.eSecurity = E_MI_WLAN_SECURITY_WPA;
    strncpy(ConnectParam.au8Password, g_pwd, MI_WLAN_MAX_PASSWD_LEN);

    //开启wifi热点
    if (g_stOpenParam.eNetworkType == E_MI_WLAN_NETWORKTYPE_AP)
    {
        strncpy(ConnectParam.au8SSId, g_ssid, MI_WLAN_MAX_SSID_LEN - 10);
        strcat(ConnectParam.au8SSId, "_ssd2xx");

        if (MI_SUCCESS == MI_WLAN_Connect(&g_wlanHdl, &ConnectParam))
        {
            g_is_connect = 1;
        }

        return;
    }

    strncpy(ConnectParam.au8SSId, g_ssid, MI_WLAN_MAX_SSID_LEN);

    //连接wifi热点
    MI_WLAN_Connect(&g_wlanHdl, &ConnectParam);

    MI_WLAN_Status_t status;
    int count = 20;
    while (1)
    {

        MI_WLAN_GetStatus(g_wlanHdl, &status);

        if (status.stStaStatus.state == WPA_COMPLETED)
        {
            g_is_connect = 1;
            printf("connect success: ssid=%s, ip=%s\n", (char *)status.stStaStatus.ssid, (char *)status.stStaStatus.ip_address);
            break;
        }

        if (--count <= 0)
        {
            printf("connect failed, timeout\n");
            break;
        }
        usleep(500 * 1000);
    }
}

void wifi_exe_disconnect()
{
    printf("[CMD]:disconnect & close\n");

    if (g_is_connect)
    {
        MI_WLAN_Disconnect(g_wlanHdl);
        MI_WLAN_Close(&g_stOpenParam);
        g_is_connect = 0;
    }else{
        MI_WLAN_Disconnect(NULL);
        MI_WLAN_Close(&g_stOpenParam);
    }
}

void wifi_exe_print_info()
{
    printf("[CMD]:printf current status\n");
    // MI_U8 ChipVersion;
    // MI_WLAN_GetWlanChipVersion(&ChipVersion);
    // printf("ChipVersion: %d\n",ChipVersion);

    if (!g_is_connect)
    {
        printf("wlan is disconnected, please connect first\n");
        return;
    }

    MI_WLAN_Status_t status;
    memset(&status,0,sizeof(status));
    if (g_stOpenParam.eNetworkType == E_MI_WLAN_NETWORKTYPE_AP)
    {
        MI_WLAN_GetStatus(g_wlanHdl, &status);
        printf("Access port:\n");

        for (int i = 0; i < status.stApStatus.u16HostNum; i++)
        {
            printf("HostName: %s\n", status.stApStatus.astHosts[i].hostname);
            printf("IP: %s\n", (char *)status.stApStatus.astHosts[i].ipaddr);
            printf("MAC: %s\n", (char *)status.stApStatus.astHosts[i].macaddr);
            printf("Connected time: %lld\n", status.stApStatus.astHosts[i].connectedtime);
            printf("\n");
        }
        return;
    }

    MI_WLAN_ScanResult_t scanResult;
    memset(&scanResult,0,sizeof(scanResult));

    MI_WLAN_Scan(NULL, &scanResult);
    printf("Scan result:\n");

    for (int i = 0; i < scanResult.u8APNumber; i++)
    {
        char *pSsid = (char *)scanResult.stAPInfo[i].au8SSId;
        if (pSsid && strcmp(pSsid, "\"\""))
        {
            char trimSsid[36];
            memset(trimSsid, 0, sizeof(trimSsid));
            strncpy(trimSsid, pSsid + 1, strlen(pSsid));
            printf("SSID: %s\n", trimSsid);
            printf("MAC: %s\n", (char *)scanResult.stAPInfo[i].au8Mac);
            printf("encrypt: %s\n", scanResult.stAPInfo[i].bEncryptKey ? "true" : "false");
            printf("signalSTR: %d db\n", scanResult.stAPInfo[i].stQuality.signalSTR);
            printf("frequency: %f GHz\n", scanResult.stAPInfo[i].fFrequency);
            printf("bitrate: %f Mb/s\n", scanResult.stAPInfo[i].fBitRate);
            printf("channel: %d\n", scanResult.stAPInfo[i].u8Channel);
            printf("channel: %d\n", scanResult.stAPInfo[i].u16CellId);
            printf("\n");
        }
    }
}

static wifi_exe_t exes[] = {
    {'m', wifi_exe_switch_mode},
    {'c', wifi_exe_connect},
    {'d', wifi_exe_disconnect},
    {'p', wifi_exe_print_info},
    {'h', wifi_demo_cmd_help}

};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("eg:%s (ssid) (passworld) \n", argv[0]);
        return -1;
    }
    else
    {
        g_ssid = argv[1];
        g_pwd = argv[2];
    }

    MI_WLAN_InitParams_t g_stParam = {"/config/wifi/wlan.json"};

    if (MI_WLAN_Init(&g_stParam))
    {
        printf("Error:Wlan init failed, please ensure wlan module is enabled.\n");
        return -1;
    }
    
    wifi_demo_cmd_help();

    while (1)
    {
        char cmd;
        cmd = getchar();
        printf("wifi_demo\\");

        for (int i = 0; i < sizeof(exes) / sizeof(exes[0]); i++)
        {
            exes[i].cmd == cmd ? exes[i].func() : 1;
        }

        if (cmd == 'q')
        {
            break;
        }
    }

    MI_WLAN_DeInit();
    return 0;
}
