#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "list.h"
#include "wifi_config.h"

typedef enum
{
    WIFI_CMD_SCAN,
    WIFI_CMD_CONNECT,
    WIFI_CMD_DISCONNECT,
} WIFI_CMD_E;

typedef struct _get_cmd_node_t
{
    WIFI_CMD_E cmd;
    void *arg;
    wifi_cb_t cb;
} get_cmd_node_t;

typedef struct _cb_cmd_node_t
{

    wifi_cb_t cb;
    void *arg;
    void *req;
} cb_cmd_node_t;

typedef struct _wifi_config_info_t
{
    pthread_t p_init_id;

    pthread_mutex_t mutex; //线程同步
    wifi_cb_t open_cb;
    wifi_cb_t state_cb;
    WLAN_HANDLE hWlan;

    list_t *get_cmd_list;
    list_t *cb_cmd_list;
    int is_open; //1 打开，0未打开
} wifi_config_info_t;

wifi_config_info_t g_info;

static void *wifi_init_pthread_init(void *pdata);
static void wifi_state_run();
static void wifi_add_cb_list(wifi_cb_t cb, void *arg, void *req);

int wifi_init(wifi_cb_t open_cb, wifi_cb_t state_cb)
{
    printf("%s %d <<<<<<<<<<<<\n", __func__, __LINE__);

    memset(&g_info, 0, sizeof(g_info));

    g_info.hWlan = -1;
    g_info.open_cb = open_cb;
    g_info.state_cb = state_cb;

    g_info.get_cmd_list = list_new();
    if (!g_info.get_cmd_list)
    {
        return -1;
    }

    g_info.cb_cmd_list = list_new();
    if (!g_info.cb_cmd_list)
    {
        return -1;
    }

    pthread_mutex_init(&g_info.mutex, NULL);

    //MI 中wifi 操作是阻塞的，所以开单独个线程来避免阻塞主线程
    pthread_create(&g_info.p_init_id, NULL, wifi_init_pthread_init, NULL);
    if (!g_info.p_init_id)
    {
        printf("Error: create check wifi  thread failed\n");
        return -1;
    }

    printf("%s %d >>>>>>>>>>>>>>\n", __func__, __LINE__);
    return 0;
}

static void *wifi_init_pthread_init(void *pdata)
{
    printf("%s %d <<<<<<<<<<<<\n", __func__, __LINE__);

    int checkCnt = 40;
    int isWlanInsmode = 0;

    MI_WLAN_InitParams_t stParm = {"/config/wifi/wlan.json"};
    MI_WLAN_OpenParams_t stOpenParam = {E_MI_WLAN_NETWORKTYPE_INFRA};

    if (MI_WLAN_Init(&stParm) || MI_WLAN_Open(&stOpenParam))
    {
        printf("Error: open wifi failed\n");
        wifi_add_cb_list(g_info.open_cb, NULL, (void *)(g_info.is_open));
        return NULL;
    }
    else
    {
        g_info.is_open = 1;
        wifi_add_cb_list(g_info.open_cb, NULL, (void *)(g_info.is_open));
    }

    while (1)
    {

        wifi_state_run();

        list_node_t *node;
        get_cmd_node_t *val;

        pthread_mutex_lock(&g_info.mutex);
        node = list_lpop(g_info.get_cmd_list);
        pthread_mutex_unlock(&g_info.mutex);

        if (!node)
        {
            usleep(100 * 1000);
            continue;
        }
        val = node->val;

        MI_WLAN_ScanResult_t *scanResult;
        MI_WLAN_ConnectParam_t *pstConnParam;
        switch (val->cmd)
        {
        case WIFI_CMD_SCAN:
            scanResult = malloc(sizeof(MI_WLAN_ScanResult_t));
            MI_WLAN_Scan(NULL, scanResult);
            wifi_add_cb_list(val->cb, val->arg, scanResult);
            break;
        case WIFI_CMD_CONNECT:
            pstConnParam = val->arg;
            if (pstConnParam)
            {
                MI_WLAN_Connect(&g_info.hWlan, pstConnParam);
                free(pstConnParam);
            }
            break;
        case WIFI_CMD_DISCONNECT:
            if (g_info.hWlan > 0)
            {
                MI_WLAN_Disconnect(g_info.hWlan);
                g_info.hWlan = -1;
            }
            wifi_add_cb_list(val->cb, val->arg, NULL);
            break;
        default:

            break;
        }

        free(val);
        free(node);

        usleep(50 * 1000);
    }

    printf("%s %d >>>>>>>>>>>>>>\n", __func__, __LINE__);
    return NULL;
}

//添加应答节点
static void wifi_add_cb_list(wifi_cb_t cb, void *arg, void *req)
{
    cb_cmd_node_t *val = malloc(sizeof(cb_cmd_node_t));
    val->arg = arg;
    val->req = req;
    val->cb = cb;
    list_node_t *node = list_node_new((void *)val);

    pthread_mutex_lock(&g_info.mutex);
    list_rpush(g_info.cb_cmd_list, node);
    pthread_mutex_unlock(&g_info.mutex);
}

static void wifi_state_run()
{
    static MI_WLAN_Status_sta_t stStaStatus_old;
    MI_WLAN_Status_t stStatus;
    if (g_info.hWlan < 0)
    {
        return;
    }
    MI_WLAN_GetStatus(g_info.hWlan, &stStatus);
    if (0 == memcmp(&stStaStatus_old, &stStatus.stStaStatus, sizeof(MI_WLAN_Status_sta_t)))
    {
        return;
    }

    MI_WLAN_Status_sta_t *st = malloc(sizeof(MI_WLAN_Status_sta_t));
    memcpy(st, &stStatus, sizeof(MI_WLAN_Status_sta_t));
    memcpy(&stStaStatus_old, &stStatus, sizeof(MI_WLAN_Status_sta_t));

    wifi_add_cb_list(g_info.state_cb, NULL, (void *)st);
}

//主线程调用，搜索完成后通过全局list通知主线程
void wifi_scan(wifi_cb_t cb, void *arg)
{

    get_cmd_node_t *val = malloc(sizeof(get_cmd_node_t));
    val->arg = arg;
    val->cmd = WIFI_CMD_SCAN;
    val->cb = cb;
    list_node_t *node = list_node_new((void *)val);

    pthread_mutex_lock(&g_info.mutex);
    list_rpush(g_info.get_cmd_list, node);
    pthread_mutex_unlock(&g_info.mutex);
}

void wifi_connect(char *ssid, char *pwd)
{
    if (!ssid || !pwd)
    {
        return;
    }
    MI_WLAN_ConnectParam_t *pstConnParam = malloc(sizeof(MI_WLAN_ConnectParam_t));
    memset(pstConnParam, 0, sizeof(MI_WLAN_ConnectParam_t));
    pstConnParam->eSecurity = E_MI_WLAN_SECURITY_WPA;
    pstConnParam->OverTimeMs = 5 * 1000;

    strcpy((char *)pstConnParam->au8SSId, ssid);
    strcpy((char *)pstConnParam->au8Password, pwd);

    get_cmd_node_t *val = malloc(sizeof(get_cmd_node_t));
    val->arg = (void *)pstConnParam;
    val->cmd = WIFI_CMD_CONNECT;
    val->cb = NULL;
    list_node_t *node = list_node_new((void *)val);

    pthread_mutex_lock(&g_info.mutex);
    list_rpush(g_info.get_cmd_list, node);
    pthread_mutex_unlock(&g_info.mutex);
}

void wifi_disconnect(wifi_cb_t cb, void *arg)
{
    get_cmd_node_t *val = malloc(sizeof(get_cmd_node_t));
    val->arg = arg;
    val->cmd = WIFI_CMD_DISCONNECT;
    val->cb = cb;
    list_node_t *node = list_node_new((void *)val);

    pthread_mutex_lock(&g_info.mutex);
    list_rpush(g_info.get_cmd_list, node);
    pthread_mutex_unlock(&g_info.mutex);
}

void wifi_mastar_run()
{
    pthread_mutex_lock(&g_info.mutex);
    list_node_t *node = list_lpop(g_info.cb_cmd_list);
    pthread_mutex_unlock(&g_info.mutex);

    if (!node)
    {
        return;
    }
    cb_cmd_node_t *val = node->val;
    if (val->cb)
    {
        val->cb(val->arg, val->req);
    }
    free(val);
    free(node);
}
