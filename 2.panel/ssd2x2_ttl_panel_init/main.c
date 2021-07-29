#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp.h"


int main(int argc, char **argv)
{
	MI_PANEL_IntfType_e eIntfType;
	eIntfType = E_MI_PNL_INTF_TTL;
	MI_PANEL_Init(eIntfType); //panel 初始化
    MI_DISP_Enable(0);        //fb最终还是得经过disp，所以得初始化disp

    while(1);
    //MI_PANEL_DeInit();
	return 0;
}
