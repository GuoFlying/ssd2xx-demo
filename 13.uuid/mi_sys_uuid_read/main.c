#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mi_sys.h"
#include "mi_common_datatype.h"

int main(int argc, char *argv[])
{
    union funion_data
    {
        uint64_t uuid;
        uint8_t byte[8];
    } f_data;

    MI_S32 s32Ret = MI_ERR_SYS_FAILED;
    s32Ret = MI_SYS_ReadUuid(&f_data.uuid);
    if (s32Ret != 0)
    {
        printf("error: MI_SYS_ReadUuid\n");
        return -1;
    }

    char cmd[50] = "";
    sprintf(cmd, "ifconfig eth0 hw ether %02x:%02x:%02x:%02x:%02x:%02x",
            f_data.byte[5], f_data.byte[4], f_data.byte[3], f_data.byte[2], f_data.byte[1], f_data.byte[0]);

    system(cmd);
    printf("uuid: %llx\n", f_data.uuid);
    printf("cmd: %s\n", cmd);

    //app退出，log打印“[MI ERR ]: MI_SYS_IMPL_Exit[3821]: gSysInitCount:1” 正常
    sleep(1);
    return 0;
}