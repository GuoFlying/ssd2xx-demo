#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "mi_sys.h"
#include "sstardisp.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"


#include "SAT070BO30I21Y0_1024x600_MIPI.h"
//#include "AM700D32_30AC_N01_1024X600_MIPI.h"

//#define LOCAL_VIDEO_W  640
//#define LOCAL_VIDEO_H  360
//#define LOCAL_VIDEO_X  100
//#define LOCAL_VIDEO_Y  60
#define LOCAL_VIDEO_W  1024
#define LOCAL_VIDEO_H  600
#define LOCAL_VIDEO_X  0
#define LOCAL_VIDEO_Y  0



#define MAKE_YUYV_VALUE(y,u,v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0,128,128)

int sstar_disp_init(MI_DISP_PubAttr_t *pstDispPubAttr)
{
    MI_PANEL_LinkType_e eLinkType;
    MI_DISP_InputPortAttr_t stInputPortAttr;

    MI_DISP_VideoLayerAttr_t stLayerAttr;

    memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));

    MI_SYS_Init();

    if (pstDispPubAttr->eIntfType == E_MI_DISP_INTF_LCD)
    {
        pstDispPubAttr->stSyncInfo.u16Vact = stPanelParam.u16Height;
        pstDispPubAttr->stSyncInfo.u16Vbb = stPanelParam.u16VSyncBackPorch;
        pstDispPubAttr->stSyncInfo.u16Vfb = stPanelParam.u16VTotal - (stPanelParam.u16VSyncWidth +
                                                                      stPanelParam.u16Height + stPanelParam.u16VSyncBackPorch);
        pstDispPubAttr->stSyncInfo.u16Hact = stPanelParam.u16Width;
        pstDispPubAttr->stSyncInfo.u16Hbb = stPanelParam.u16HSyncBackPorch;
        pstDispPubAttr->stSyncInfo.u16Hfb = stPanelParam.u16HTotal - (stPanelParam.u16HSyncWidth +
                                                                      stPanelParam.u16Width + stPanelParam.u16HSyncBackPorch);
        pstDispPubAttr->stSyncInfo.u16Bvact = 0;
        pstDispPubAttr->stSyncInfo.u16Bvbb = 0;
        pstDispPubAttr->stSyncInfo.u16Bvfb = 0;
        pstDispPubAttr->stSyncInfo.u16Hpw = stPanelParam.u16HSyncWidth;
        pstDispPubAttr->stSyncInfo.u16Vpw = stPanelParam.u16VSyncWidth;
        pstDispPubAttr->stSyncInfo.u32FrameRate = stPanelParam.u16DCLK * 1000000 / (stPanelParam.u16HTotal * stPanelParam.u16VTotal);
        pstDispPubAttr->eIntfSync = E_MI_DISP_OUTPUT_USER;
        pstDispPubAttr->eIntfType = E_MI_DISP_INTF_LCD;

		eLinkType = E_MI_PNL_LINK_MIPI_DSI;

        stInputPortAttr.u16SrcWidth = LOCAL_VIDEO_W;
        stInputPortAttr.u16SrcHeight = LOCAL_VIDEO_H;
        stInputPortAttr.stDispWin.u16X = LOCAL_VIDEO_X;
        stInputPortAttr.stDispWin.u16Y = LOCAL_VIDEO_Y;
        stInputPortAttr.stDispWin.u16Width = LOCAL_VIDEO_W;
        stInputPortAttr.stDispWin.u16Height = LOCAL_VIDEO_H;

        MI_DISP_SetPubAttr(0, pstDispPubAttr);
        MI_DISP_Enable(0);
        MI_DISP_BindVideoLayer(0, 0);
		memset(&stLayerAttr, 0, sizeof(stLayerAttr));
        stLayerAttr.stVidLayerSize.u16Width  = LOCAL_VIDEO_W;
        stLayerAttr.stVidLayerSize.u16Height = LOCAL_VIDEO_H;
        stLayerAttr.ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_MST_420;
        stLayerAttr.stVidLayerDispWin.u16X      = 0;
        stLayerAttr.stVidLayerDispWin.u16Y      = 0;
        stLayerAttr.stVidLayerDispWin.u16Width  = LOCAL_VIDEO_W;
        stLayerAttr.stVidLayerDispWin.u16Height = LOCAL_VIDEO_H;
        MI_DISP_SetVideoLayerAttr(0, &stLayerAttr);
        MI_DISP_EnableVideoLayer(0);

        MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
        MI_DISP_EnableInputPort(0, 0);
        MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);	
        //MI_DISP_RotateConfig_t pstRotateConfig={E_MI_DISP_ROTATE_90};
        //MI_DISP_SetVideoLayerRotateMode(0,&pstRotateConfig);
    }

    if (pstDispPubAttr->eIntfType == E_MI_DISP_INTF_LCD)
    {
        MI_PANEL_Init(eLinkType);
        MI_PANEL_SetPanelParam(&stPanelParam);
        if(eLinkType == E_MI_PNL_LINK_MIPI_DSI)
        {

            // printf("\n[Jack]/config/riu_w 0x101e 0x0d 0x1000############ \n");
            // system("/config/riu_w 0x101e 0x0d 0x1000");
            // system("/config/riu_w 0x1129 0x18 0x0581");
            MI_PANEL_SetMipiDsiConfig(&stMipiDsiConfig);

        }
    }
    

    return 0;
}
int sstar_disp_Deinit(MI_DISP_PubAttr_t *pstDispPubAttr)
{

    MI_DISP_DisableInputPort(0, 0);
    MI_DISP_DisableVideoLayer(0);
    MI_DISP_UnBindVideoLayer(0, 0);
    MI_DISP_Disable(0);

    switch(pstDispPubAttr->eIntfType) {

        case E_MI_DISP_INTF_VGA:
            break;

        case E_MI_DISP_INTF_LCD:
        default:
            MI_PANEL_DeInit();

    }

    MI_SYS_Exit();
    printf("sstar_disp_Deinit...\n");

    return 0;
}


