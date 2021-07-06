#ifndef PNL_TABLE_SAT070BO30I21Y0_1440
#define PNL_TABLE_SAT070BO30I21Y0_1440
#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER

MI_PANEL_ParamConfig_t stPanelParam =
{
    "SAT070BO30I21Y0_1440", // const char *m_pPanelName;                ///<  PanelName
    0, //MS_U8 m_bPanelDither :1;                 ///<  PANEL_DITHER, keep the setting
    E_MI_PNL_LINK_MIPI_DSI, //MHAL_DISP_ApiPnlLinkType_e m_ePanelLinkType   :4;  ///<  PANEL_LINK

    ///////////////////////////////////////////////
    // Board related setting
    ///////////////////////////////////////////////
    1,  //MS_U8 m_bPanelDualPort  :1;              ///<  VOP_21[8], MOD_4A[1],    PANEL_DUAL_PORT, refer to m_bPanelDoubleClk
    0,  //MS_U8 m_bPanelSwapPort  :1;              ///<  MOD_4A[0],               PANEL_SWAP_PORT, refer to "LVDS output app note" A/B channel swap
    0,  //MS_U8 m_bPanelSwapOdd_ML    :1;          ///<  PANEL_SWAP_ODD_ML
    0,  //MS_U8 m_bPanelSwapEven_ML   :1;          ///<  PANEL_SWAP_EVEN_ML
    0,  //MS_U8 m_bPanelSwapOdd_RB    :1;          ///<  PANEL_SWAP_ODD_RB
    0,  //MS_U8 m_bPanelSwapEven_RB   :1;          ///<  PANEL_SWAP_EVEN_RB

    0,  //MS_U8 m_bPanelSwapLVDS_POL  :1;          ///<  MOD_40[5], PANEL_SWAP_LVDS_POL, for differential P/N swap
    0,  //MS_U8 m_bPanelSwapLVDS_CH   :1;          ///<  MOD_40[6], PANEL_SWAP_LVDS_CH, for pair swap
    0,  //MS_U8 m_bPanelPDP10BIT      :1;          ///<  MOD_40[3], PANEL_PDP_10BIT ,for pair swap
    1,  //MS_U8 m_bPanelLVDS_TI_MODE  :1;          ///<  MOD_40[2], PANEL_LVDS_TI_MODE, refer to "LVDS output app note"

    ///////////////////////////////////////////////
    // For TTL Only
    ///////////////////////////////////////////////
    0,  //MS_U8 m_ucPanelDCLKDelay;                ///<  PANEL_DCLK_DELAY
    0,  //MS_U8 m_bPanelInvDCLK   :1;              ///<  MOD_4A[4],                   PANEL_INV_DCLK
    0,  //MS_U8 m_bPanelInvDE     :1;              ///<  MOD_4A[2],                   PANEL_INV_DE
    0,  //MS_U8 m_bPanelInvHSync  :1;              ///<  MOD_4A[12],                  PANEL_INV_HSYNC
    0,  //MS_U8 m_bPanelInvVSync  :1;              ///<  MOD_4A[3],                   PANEL_INV_VSYNC

    ///////////////////////////////////////////////
    // Output driving current setting
    ///////////////////////////////////////////////
    // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
    1,  //MS_U8 m_ucPanelDCKLCurrent;              ///<  define PANEL_DCLK_CURRENT
    1,  //MS_U8 m_ucPanelDECurrent;                ///<  define PANEL_DE_CURRENT
    1,  //MS_U8 m_ucPanelODDDataCurrent;           ///<  define PANEL_ODD_DATA_CURRENT
    1,  //MS_U8 m_ucPanelEvenDataCurrent;          ///<  define PANEL_EVEN_DATA_CURRENT

    ///////////////////////////////////////////////
    // panel on/off timing
    ///////////////////////////////////////////////
    30,  //MS_U16 m_wPanelOnTiming1;                ///<  time between panel & data while turn on power
    400,  //MS_U16 m_wPanelOnTiming2;                ///<  time between data & back light while turn on power
    80,  //MS_U16 m_wPanelOffTiming1;               ///<  time between back light & data while turn off power
    30,  //MS_U16 m_wPanelOffTiming2;               ///<  time between data & panel while turn off power

    ///////////////////////////////////////////////
    // panel timing spec.
    ///////////////////////////////////////////////
    // sync related
    10,  //MS_U8 m_ucPanelHSyncWidth;               ///<  VOP_01[7:0], PANEL_HSYNC_WIDTH
    60,  //MS_U8 m_ucPanelHSyncBackPorch;           ///<  PANEL_HSYNC_BACK_PORCH, no register setting, provide value for query only

                                             ///<  not support Manuel VSync Start/End now
                                             ///<  VOP_02[10:0] VSync start = Vtt - VBackPorch - VSyncWidth
                                             ///<  VOP_03[10:0] VSync end = Vtt - VBackPorch
    10,  //MS_U8 m_ucPanelVSyncWidth;               ///<  define PANEL_VSYNC_WIDTH
    8,  //MS_U8 m_ucPanelVBackPorch;               ///<  define PANEL_VSYNC_BACK_PORCH

    // DE related
    70,  //MS_U16 m_wPanelHStart;                   ///<  VOP_04[11:0], PANEL_HSTART, DE H Start (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
    18,  //MS_U16 m_wPanelVStart;                   ///<  VOP_06[11:0], PANEL_VSTART, DE V Start
    1440,  //MS_U16 m_wPanelWidth;                    ///< PANEL_WIDTH, DE width (VOP_05[11:0] = HEnd = HStart + Width - 1)
    900,  //MS_U16 m_wPanelHeight;                   ///< PANEL_HEIGHT, DE height (VOP_07[11:0], = Vend = VStart + Height - 1)

    // DClk related
    1650,  //MS_U16 m_wPanelMaxHTotal;                ///<  PANEL_MAX_HTOTAL. Reserved for future using.
    1570,  //MS_U16 m_wPanelHTotal;                   ///<  VOP_0C[11:0], PANEL_HTOTAL
    1500,  //MS_U16 m_wPanelMinHTotal;                ///<  PANEL_MIN_HTOTAL. Reserved for future using.

    1000,  //MS_U16 m_wPanelMaxVTotal;                ///<  PANEL_MAX_VTOTAL. Reserved for future using.
    924,  //MS_U16 m_wPanelVTotal;                   ///<  VOP_0D[11:0], PANEL_VTOTAL
    850,  //MS_U16 m_wPanelMinVTotal;                ///<  PANEL_MIN_VTOTAL. Reserved for future using.

    90,  //MS_U8 m_dwPanelMaxDCLK;                  ///<  PANEL_MAX_DCLK. Reserved for future using.
    87,  //MS_U8 m_dwPanelDCLK;                     ///<  LPLL_0F[23:0], PANEL_DCLK          ,{0x3100_10[7:0], 0x3100_0F[15:0]}
    84,  //MS_U8 m_dwPanelMinDCLK;                  ///<  PANEL_MIN_DCLK. Reserved for future using.
                                             ///<  spread spectrum
    2130,  //MS_U16 m_wSpreadSpectrumStep;            ///<  move to board define, no use now.
    163,  //MS_U16 m_wSpreadSpectrumSpan;            ///<  move to board define, no use now.

    160,  //MS_U8 m_ucDimmingCtl;                    ///<  Initial Dimming Value
    255,  //MS_U8 m_ucMaxPWMVal;                     ///<  Max Dimming Value
    80,  //MS_U8 m_ucMinPWMVal;                     ///<  Min Dimming Value

    0,  //MS_U8 m_bPanelDeinterMode   :1;          ///<  define PANEL_DEINTER_MODE,  no use now
    E_MI_PNL_ASPECT_RATIO_WIDE,  //MHAL_DISP_PnlAspectRatio_e m_ucPanelAspectRatio; ///<  Panel Aspect Ratio, provide information to upper layer application for aspect ratio setting.
  /*
    *
    * Board related params
    *
    *  If a board ( like BD_MST064C_D01A_S ) swap LVDS TX polarity
    *    : This polarity swap value =
    *      (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L from board define,
    *  Otherwise
    *    : The value shall set to 0.
    */
    0,  //MS_U16 m_u16LVDSTxSwapValue;
    E_MI_PNL_TI_8BIT_MODE,  //MHAL_DISP_ApiPnlTiBitMode_e m_ucTiBitMode;                         ///< MOD_4B[1:0], refer to "LVDS output app note"
    E_MI_PNL_OUTPUT_8BIT_MODE,  //MHAL_DISP_ApiPnlOutPutFormatBitMode_e m_ucOutputFormatBitMode;

    0,  //MS_U8 m_bPanelSwapOdd_RG    :1;          ///<  define PANEL_SWAP_ODD_RG
    0,  //MS_U8 m_bPanelSwapEven_RG   :1;          ///<  define PANEL_SWAP_EVEN_RG
    0,  //MS_U8 m_bPanelSwapOdd_GB    :1;          ///<  define PANEL_SWAP_ODD_GB
    0,  //MS_U8 m_bPanelSwapEven_GB   :1;          ///<  define PANEL_SWAP_EVEN_GB

    /**
    *  Others
    */
    1,  //MS_U8 m_bPanelDoubleClk     :1;             ///<  LPLL_03[7], define Double Clock ,LVDS dual mode
    0x001c848e,  //MS_U32 m_dwPanelMaxSET;                     ///<  define PANEL_MAX_SET
    0x0018eb59,  //MS_U32 m_dwPanelMinSET;                     ///<  define PANEL_MIN_SET
    E_MI_PNL_CHG_VTOTAL,  //MHAL_DISP_ApiPnlOutTimingMode_e m_ucOutTimingMode;   ///<Define which panel output timing change mode is used to change VFreq for same panel
    0,  //MS_U8 m_bPanelNoiseDith     :1;             ///<  PAFRC mixed with noise dither disable
    E_MI_PNL_CH_SWAP_2,
    E_MI_PNL_CH_SWAP_0,
    E_MI_PNL_CH_SWAP_1,
    E_MI_PNL_CH_SWAP_3,
    E_MI_PNL_CH_SWAP_4,
}; 

unsigned char FITI_SAT070BO30I21Y0_CMD[] =
{
    0x01, 0x00, 0x00,
    FLAG_DELAY, FLAG_DELAY, 50,
    0xb7,2, 0x50, 0x00,
    0xb8,2, 0x00, 0x00,
    0xb9,2, 0x00, 0x00,
    0xba,2, 0x14, 0x42,
    0xbb,2, 0x03, 0x00,
    0xb9,2, 0x01, 0x00,
    0xde,2, 0x03, 0x00,
    0xc9,2, 0x02, 0x23,
    0xb7,2, 0x10, 0x02,
    0xbd,2, 0x00, 0x00,
    0x11, 0x00, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0x29, 0x00, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,
};

MI_PANEL_MipiDsiConfig_t stMipiDsiConfig =
{
    //HsTrail HsPrpr HsZero ClkHsPrpr ClkHsExit ClkTrail ClkZero ClkHsPost DaHsExit ContDet
    5,      4,     8,     4,       7,       4,       18,     11,       7,       0,
    //Lpx   TaGet  TaSure  TaGo
    16,   26,    24,     50,

    //Hac,  Hpw,  Hbp,  Hfp,  Vac,  Vpw, Vbp, Vfp,  Bllp, Fps
    1440,  10,    60,   60,   900, 10,   8,  6,  0,    60,

    E_MI_PNL_MIPI_DSI_LANE_4,      // MhalPnlMipiDsiLaneMode_e enLaneNum;
    E_MI_PNL_MIPI_DSI_RGB888,      // MhalPnlMipiDsiFormat_e enFormat;
    E_MI_PNL_MIPI_DSI_SYNC_PULSE,  // MhalPnlMipiDsiCtrlMode_e enCtrl;

    FITI_SAT070BO30I21Y0_CMD,
    sizeof(FITI_SAT070BO30I21Y0_CMD),
    1, 0x01AF, 0x01B9, 0x80D2, 9,
};
#endif //PNL_TABLE_SAT070BO30I21Y0