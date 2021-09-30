#ifndef LV_F_GAME_TETRIS_H
#define LV_F_GAME_TETRIS_H

#include "../../lvgl/lvgl.h"
#include "tetris_logic/tetris_logic.h"

#define F_TETRIS_STAGE_H (LV_VER_RES-LV_VER_RES/5)
#define F_TETRIS_STAGE_W (LV_HOR_RES/3)
#define F_TETRIS_STAGE_BORDER  10/*边框*/
#define F_TETRIS_REAL_STAGE_H (F_TETRIS_STAGE_H-F_TETRIS_STAGE_BORDER*2)
#define F_TETRIS_REAL_STAGE_W (F_TETRIS_STAGE_W-F_TETRIS_STAGE_BORDER*2)

#define F_TETRIS_STAGE_LINE  2/*小方块线长*/

#define F_TETRIS_STAGE_MAP_W MAP_WIDTH /*方块地图宽大小*/
#define F_TETRIS_STAGE_MAP_H MAP_HEIGHT /*方块地图高大小*/

#define F_TETRIS_BTN_H (F_TETRIS_STAGE_H/2)
#define F_TETRIS_BTN_W (F_TETRIS_STAGE_W-40)
void f_game_tetris();

#endif