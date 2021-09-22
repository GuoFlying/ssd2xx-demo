#ifndef LV_F_2048_GAME_H
#define LV_F_2048_GAME_H
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* 大小 */
#define SIZE 4

/* 布局 */
#define LV_F_2048_NUMBER_BOX_W (70)
#define LV_F_2048_NUMBER_BOX_H (70)
#define LV_F_2048_GAME_BOX_W ((LV_F_2048_NUMBER_BOX_W + 20) * SIZE)
#define LV_F_2048_GAME_BOX_H ((LV_F_2048_NUMBER_BOX_H + 20) * SIZE)

/* 颜色 */
#define LV_F_2048_GAME_BG_COLOR lv_color_hex(0xe8e5db) //LV_COLOR_MAKE(0xCD, 0xC1, 0xB4)
#define LV_F_2048_GAME_BOX_COLOR lv_color_hex(0xBBADA0)
#define LV_F_2048_NUMBER_BOX_COLOR lv_color_hex(0xCDC1B4)
#define LV_F_2048_NUMBER_2_COLOR lv_color_hex(0xeee4da)
#define LV_F_2048_NUMBER_4_COLOR lv_color_hex(0xeee1c9)
#define LV_F_2048_NUMBER_8_COLOR lv_color_hex(0xf3b27a)
#define LV_F_2048_NUMBER_16_COLOR lv_color_hex(0xf69664)
#define LV_F_2048_NUMBER_32_COLOR lv_color_hex(0xf77c5f)
#define LV_F_2048_NUMBER_64_COLOR lv_color_hex(0xf75f3b)
#define LV_F_2048_NUMBER_128_COLOR lv_color_hex(0xedd073)
#define LV_F_2048_NUMBER_256_COLOR lv_color_hex(0xEDCC61)
#define LV_F_2048_NUMBER_512_COLOR lv_color_hex(0xEDCC61)
#define LV_F_2048_NUMBER_1024_COLOR lv_color_hex(0xEDCC61)
#define LV_F_2048_NUMBER_2048_COLOR lv_color_hex(0xEDC22E)

    /**********************
 *      TYPEDEFS
 **********************/
    typedef struct _lv_f_2048_game
    {
        lv_obj_t *bg;                  /* 背景 */
        lv_obj_t *canvas_stage;        /* 游戏舞台 */
        lv_obj_t *label_best_score;    /* 展示最高分数 */
        lv_obj_t *label_current_score; /* 展示当前分数 */
        lv_obj_t *task_handle;         /* 任务句柄 */

        uint8_t game_board[SIZE][SIZE]; /* 2048矩阵 */
        uint32_t current_score_value;   /* 当前分数 */
        bool play_game;                 /* 游戏状态 */
    } T_lv_f_2048_game, *PT_lv_f_2048_game;

    /**********************
 * GLOBAL PROTOTYPES
 **********************/
    void lv_f_game_2048(void);

    /**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_F_2048_GAME_H */
