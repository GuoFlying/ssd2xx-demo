/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _MI_FB_DATATYPE_H_
#define _MI_FB_DATATYPE_H_
#include "mi_common.h"

typedef enum
{
    /// Color format RGB565.
    E_MI_FB_COLOR_FMT_RGB565          =1,
    /// Color format ARGB4444.
    E_MI_FB_COLOR_FMT_ARGB4444        =2,
    /// Color format I8
    E_MI_FB_COLOR_FMT_I8              =4,
    /// Color format ARGB8888.
    E_MI_FB_COLOR_FMT_ARGB8888        =5,
    /// Color format ARGB1555.
    E_MI_FB_COLOR_FMT_ARGB1555        =6,
    /// Color format YUV422.
    E_MI_FB_COLOR_FMT_YUV422          =9,
    /// Color format I4
    E_MI_FB_COLOR_FMT_I4              =13,
    /// Color format I2
    E_MI_FB_COLOR_FMT_I2              =14,
    /// Invalid color format.
    E_MI_FB_COLOR_FMT_INVALID,
}MI_FB_ColorFmt_e;

typedef enum
{
    E_MI_FB_OUTPUT_RGB = 0,
    E_MI_FB_OUTPUT_YUV = 1
}MI_FB_OutputColorSpace_e;

typedef enum
{
    E_MI_FB_DST_OP0 = 0,
    E_MI_FB_DST_OP1 = 1,
    E_MI_FB_DST_INVALID
}MI_FB_DstDisplayplane_e;

typedef enum
{
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_DISP_POS = 0x1,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_DISP_SIZE = 0x2,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_BUFFER_SIZE = 0x4,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_SCREEN_SIZE = 0x8,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_PREMUL = 0x10,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_COLOR_FMB = 0x20,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_OUTPUT_COLORSPACE = 0x40,
    E_MI_FB_DISPLAYLAYER_ATTR_MASK_DST_DISP = 0x80,
}MI_FB_DisplayLayerAttrMaskbit_e;

typedef enum
{
    E_MI_FB_CURSOR_ATTR_MASK_ICON = 0x1,
    E_MI_FB_CURSOR_ATTR_MASK_POS = 0x2,
    E_MI_FB_CURSOR_ATTR_MASK_ALPHA = 0x4,
    E_MI_FB_CURSOR_ATTR_MASK_SHOW = 0x8,
    E_MI_FB_CURSOR_ATTR_MASK_HIDE = 0x10,
    E_MI_FB_CURSOR_ATTR_MASK_COLORKEY = 0x20,
    E_MI_FB_CURSOR_ATTR_MASK = 0x3F
}MI_FB_CursorAttrMaskbit_e;

typedef struct
{
    MI_U32 u32Offset;
    MI_U32 u32Length;
    MI_U32 u32MsbRight;
}MI_FB_BitField_t;

typedef struct
{
    MI_U32 u32Xres;                /* visible resolution */
    MI_U32 u32Yres;
    MI_U32 u32Xres_virtual;         /* virtual resolution */
    MI_U32 u32Yres_virtual;
    MI_U32 u32Xoffset;             /* offset from virtual to visible */
    MI_U32 u32Yoffset;             /* resolution */

    MI_U32 u32BitsPerPixel;         /* guess what */
    MI_U32 u32Grayscale;           /* != 0 Graylevels instead of colors */

    MI_FB_BitField_t stBitFieldRed;            /* bitfield in fb mem if true color, */
    MI_FB_BitField_t stBitFieldGreen;        /* else only length is significant */
    MI_FB_BitField_t stBitFieldBlue;
    MI_FB_BitField_t stBitFieldTransp;        /* transparency */

    MI_U32 u32Nonstd;            /* != 0 Non standard pixel format */
    MI_U32 u32Rotate;             /* angle we rotate counter clockwise */
    MI_U32 u32Reserved[5];        /* Reserved for future compatibility */
}MI_FB_VarScreenInfo_t;

typedef struct
{
    MI_U8 u8Id[16];               /* identification string eg "TT Builtin" */
    MI_PHY phySmemStart;         /* Start of frame buffer mem (physical address) */
    MI_U32 u32SmemLen;           /* Length of frame buffer mem */
    MI_U32 u32Type;               /* see FB_TYPE_* */
    MI_U32 u32TypeAux;            /* Interleave for interleaved Planes */
    MI_U32 u32Visual;               /* see FB_VISUAL_* */
    MI_U16 u16XpanStep;            /* zero if no hardware panning */
    MI_U16 u16YpanStep;            /* zero if no hardware panning */
    MI_U16 u16YwrapStep;           /* zero if no hardware ywrap */
    MI_U32 u32LineLength;           /* length of a line in bytes */
    MI_PHY phyMmioStart;            /* Start of Memory Mapped I/O (physical address) */
    MI_U32 u32MmioLen;             /* Length of Memory Mapped I/O */
    MI_U32 u32Accel;               /* Indicate to driver which specific chip/card we have */
    MI_U16 u16Reserved[3];        /* Reserved for future compatibility */
}MI_FB_FixScreenInfo_t;

typedef struct MI_FB_DisplayLayerAttr_s
{
    MI_U32 u32XPos;         /**the x pos of orign point in screen*/
    MI_U32 u32YPos;         /**the y pos of orign point in screen*/
    MI_U32 u32DstWidth;      /**display buffer dest with in screen*/
    MI_U32 u32DstHeight;     /**display buffer dest hight in screen*/
    MI_U32 u32DisplayWidth;  /**the width of display buf in fb */
    MI_U32 u32DisplayHeight;  /**the height of display buf in fb. */
    MI_U32 u32ScreenWidth;   /**the width of screen */
    MI_U32 u32ScreenHeight;  /** the height of screen */
    MI_BOOL bPreMul;        /**the data drawed in buffer whether is premultiply alpha or not*/
    MI_FB_ColorFmt_e eFbColorFmt;     /**the color format of framebuffer*/
    MI_FB_OutputColorSpace_e  eFbOutputColorSpace;   /**output color space*/
    MI_FB_DstDisplayplane_e  eFbDestDisplayPlane;   /**destination displayplane*/
    MI_U32 u32SetAttrMask;    /** display attribute modify mask*/
}MI_FB_DisplayLayerAttr_t;

typedef struct MI_FB_GlobalAlpha_s
{
    MI_BOOL bAlphaEnable;      /* alpha enable flag */
    MI_BOOL bAlphaChannel;     /* alpha channel enable flag */
    MI_U8 u8Alpha0;             /* alpha0 value */
    MI_U8 u8Alpha1;             /* alpha1 value */
    MI_U8 u8GlobalAlpha;        /* global alpha value */
    MI_U8 u8Reserved;
} MI_FB_GlobalAlpha_t;

typedef struct MI_FB_ColorKey_s
{
    MI_BOOL bKeyEnable;
    MI_U8 u8Red;
    MI_U8 u8Green;
    MI_U8 u8Blue;
} MI_FB_ColorKey_t;

typedef struct MI_FB_CursorImage_s
{
    MI_U32 u32Width;     /**width, unit pixel*/
    MI_U32 u32Height;    /**Height, unit pixel*/
    MI_U32 u32Pitch;     /**Pitch, unit pixel*/
    MI_FB_ColorFmt_e eColorFmt;   /**Color format*/
    const MI_U8 *pData;
}MI_FB_CursorImage_t;

typedef struct MI_FB_CursorAttr_s
{
    MI_U32 u32XPos;
    MI_U32 u32YPos;
    MI_U32 u32HotSpotX;
    MI_U32 u32HotSpotY;
    MI_FB_GlobalAlpha_t stAlpha;
    MI_FB_ColorKey_t stColorKey;
    MI_BOOL bShown;
    MI_FB_CursorImage_t stCursorImageInfo;
    MI_U16 u16CursorAttrMask;
}MI_FB_CursorAttr_t;

typedef struct MI_FB_Rectangle_s
{
    MI_U32 u32XPos;
    MI_U32 u32YPos;
    MI_U32 u32Width;
    MI_U32 u32Height;
}MI_FB_Rectangle_t;

#endif

