#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mi_sys.h"
#include "mi_divp.h"
#include "mi_disp.h"
#include "mi_gfx.h"

#define MYPLAYER_PATH "./res/MyPlayer"
#define CLT_IPC "/appconfigs/client_input"
#define SVC_IPC "/appconfigs/server_input"
typedef enum
{
    IPC_COMMAND_OPEN,
    IPC_COMMAND_CLOSE,
    IPC_COMMAND_PAUSE,
    IPC_COMMAND_RESUME,
    IPC_COMMAND_SEEK,
    IPC_COMMAND_SEEK2TIME,
    IPC_COMMAND_GET_POSITION,
    IPC_COMMAND_GET_DURATION,
    IPC_COMMAND_MAX,
    IPC_COMMAND_ACK,
    IPC_COMMAND_SET_VOLUMN,
    IPC_COMMAND_SET_MUTE,
    IPC_COMMAND_ERROR,
    IPC_COMMAND_COMPLETE,
    IPC_COMMAND_CREATE,
    IPC_COMMAND_DESTORY,
    IPC_COMMAND_EXIT,
    IPC_COMMAND_PANT
} IPC_COMMAND_TYPE;

typedef struct
{
    int x;
    int y;
    int width;
    int height;
    double misc;
    int aodev, volumn;
    int status;
    int rotate;
    char mute;
    char audio_only, video_only;
    int play_mode;
    char filePath[512];
} stPlayerData;

typedef struct
{
    unsigned int EventType;
    stPlayerData stPlData;
} IPCEvent;

int ipc_server_init()
{
    int m_valid;
    int fd;

    unlink(SVC_IPC);
    m_valid = mkfifo(SVC_IPC, 0777);
    if (m_valid < 0)
    {
        printf(SVC_IPC "non-existent!!!! \n");
        return -1;
    }
    fd = open(SVC_IPC, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU | S_IWOTH);
    printf("IPCInput m_fd = %d\n", fd);
    return fd;
}

int ipc_server_read(int fd, IPCEvent *evt)
{
    if (fd >= 0 && evt)
    {
        return read(fd, evt, sizeof(IPCEvent));
    }
    return -1;
}

void ipc_server_deinit(int fd)
{
    unlink(SVC_IPC);
    if (fd >= 0)
    {
        close(fd);
    }
}

struct shared_use_st
{
    int written; //作为一个标志，非0：表示可读，0表示可写
    bool flag;
};

int ipc_shmat_init(void **shm_addr)
{
    int shm_id = 0;
    void *shm = NULL;
    //创建共享内存
    shm_id = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shm_id < 0)
    {
        fprintf(stderr, "shmget failed\n");
        return -1;
    }

    //将共享内存连接到当前进程的地址空间
    shm = shmat(shm_id, (void *)NULL, 0);
    if (shm < 0)
    {
        fprintf(stderr, "shmat failed\n");
        return -1;
    }

    *shm_addr = (void **)shm;
    return shm_id;
}

void ipc_shmat_deinit(int shm_id, void *shm_addr)
{
    int ret;
    if (!shm_addr)
    {
        return;
    }
    //把共享内存从当前进程中分离
    ret = shmdt((void *)shm_addr);
    if (ret < 0)
    {
        fprintf(stderr, "shmdt failed\n");
    }

    //删除共享内存
    ret = shmctl(shm_id, IPC_RMID, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
    }
}

int ipc_client_init()
{
    int fd;
    fd = open(CLT_IPC, O_WRONLY | O_NONBLOCK, S_IWUSR | S_IWOTH);
    printf("IPCOutput m_fd = %d\n", fd);
    return fd;
}

int ipc_client_send(int fd, const IPCEvent *evt)
{
    if (fd < 0 || !evt)
    {
        return -1;
    }
    return write(fd, evt, sizeof(IPCEvent));
}

void ipc_client_deinit(int fd)
{
    if (fd > 0)
    {
        close(fd);
    }
}

#include "sstardisp.h"

static MI_DISP_PubAttr_t stDispPubAttr;
#define MAKE_YUYV_VALUE(y, u, v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0, 128, 128)

static FILE *player_fd = NULL;
static IPCEvent recvevt;
static IPCEvent sendevt;
static int i_server_fd, shm_id, i_client_fd;
static struct shared_use_st *shm_addr;
static struct timeval time_start, time_wait;

//强制退出应用时，关闭对应端口
static void ctrl_c_exit(MI_S32 signo)
{
    printf("exit(1) %p\r\n", player_fd);
    if (player_fd)
    {
        pclose(player_fd);
        player_fd = NULL;
    }

    memset(&sendevt, 0, sizeof(IPCEvent));
    sendevt.EventType = IPC_COMMAND_EXIT;
    ipc_client_send(i_client_fd, &sendevt);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {

        printf("eg: \r\nexport LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./res/ssd20x_player_bin/lib/\r\n");
        printf("%s  ./res/ssd20x_player_bin/MyPlayer ./res/ssd20x_player_bin/test.mp4 1024 600\r\n", argv[0]);
        return -1;
    }
    //检测退出信号
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_exit;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
    stDispPubAttr.u32BgColor = YUYV_BLACK;

    sstar_disp_init(&stDispPubAttr);

    //修改权限，运行myplayer
    char cmd_buff[50];
    sprintf(cmd_buff, "chmod a+x %s", argv[1]);
    system(cmd_buff);
    sprintf(cmd_buff, "%s &", argv[1]);
    player_fd = popen(cmd_buff, "w");
    if (NULL == player_fd)
    {
        printf("my_player is not exit!\n");
        return -1;
    }
    pclose(player_fd);
    printf("popen myplayer progress done!\n");

    memset(&recvevt, 0, sizeof(IPCEvent));

    i_server_fd = ipc_server_init();
    shm_id = ipc_shmat_init((void **)&shm_addr);

    //等待myplayer进程开启
    gettimeofday(&time_start, NULL);
    while (ipc_server_read(i_server_fd, &recvevt) <= 0 || (recvevt.EventType != IPC_COMMAND_CREATE))
    {
        usleep(10 * 1000);
        gettimeofday(&time_wait, NULL);
        if (time_wait.tv_sec - time_start.tv_sec > 2)
        {
            printf("myplayer progress create failed!\n");
            break;
        }
    }

    //若没开启成功则退出
    if (recvevt.EventType == IPC_COMMAND_CREATE)
    {
        printf("myplayer progress create success!\n");
    }
    else
    {
        ipc_server_deinit(i_server_fd);
        ipc_shmat_deinit(shm_id, shm_addr);
        shm_addr = NULL;
        shm_id = 0;
        i_server_fd = 0;
        printf("myplayer progress create error!\n");
        return -1;
    }

    i_client_fd = ipc_client_init();
    if (i_client_fd < 0)
    {
        printf("[%s %d]my_player process not start!\n", __FILE__, __LINE__);
        return -1;
    }

    memset(&sendevt, 0, sizeof(IPCEvent));
    sendevt.EventType = IPC_COMMAND_OPEN;
    strcpy(sendevt.stPlData.filePath, argv[2]);
    printf("list file name to play = %s\n", sendevt.stPlData.filePath);

    sendevt.stPlData.rotate = E_MI_DISP_ROTATE_NONE;
    sendevt.stPlData.x = 0;
    sendevt.stPlData.y = 0;
    sendevt.stPlData.width = atoi(argv[3]);
    sendevt.stPlData.height = atoi(argv[4]);
    sendevt.stPlData.aodev = 0;
    sendevt.stPlData.audio_only = false;
    sendevt.stPlData.video_only = false;
    sendevt.stPlData.play_mode = 1; // 0: 单次播放,1: 循环播放(seek to start)
    ipc_client_send(i_client_fd, &sendevt);

    memset(&recvevt, 0, sizeof(IPCEvent));
    gettimeofday(&time_start, NULL);
    while (ipc_server_read(i_server_fd, &recvevt) <= 0 || ((recvevt.EventType != IPC_COMMAND_ACK) && (recvevt.EventType != IPC_COMMAND_ERROR)))
    {
        usleep(10 * 1000);
        gettimeofday(&time_wait, NULL);
        if (time_wait.tv_sec - time_start.tv_sec > 10)
        {
            memset(&sendevt, 0, sizeof(IPCEvent));
            sendevt.EventType = IPC_COMMAND_EXIT;
            ipc_client_send(i_client_fd, &sendevt);
            break;
        }
    }
    if (recvevt.EventType == IPC_COMMAND_ACK)
    {
        printf("receive ack from my_player!\n");
        memset(&sendevt, 0, sizeof(IPCEvent));
        sendevt.EventType = IPC_COMMAND_GET_DURATION;
        ipc_client_send(i_client_fd, &sendevt);
    }
    else if (recvevt.EventType == IPC_COMMAND_ERROR)
    {
        if (recvevt.stPlData.status == -101)
            printf("请检查网络连接！\r\n");
        else if (recvevt.stPlData.status == -2)
            printf("不支持播放720P以上的视频！\r\n");
        else if (recvevt.stPlData.status == -3)
            printf("解码速度不够，请降低视频帧率！\r\n");
        else if (recvevt.stPlData.status == -4)
            printf("读取网络超时！\r\n");
        else
            printf("Other Error Occur!\r\n");
    }

    while (1)
    {
        sleep(5);
    }

    return 0;
}