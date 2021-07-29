#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "mi_disp.h"
#include "mi_sys.h"

int main(int argc, char **argv)
{
    MI_SYS_Init();
    MI_DISP_Enable(0);
    
    //应用直接退出的话，会主动disable
    //MI_DISP_Disable(0);
    //MI_SYS_Exit();
    return 0;
}
