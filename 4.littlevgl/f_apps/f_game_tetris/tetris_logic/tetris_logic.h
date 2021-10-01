
#ifndef _TETRIS_LOGIC_H_
#define _TETRIS_LOGIC_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    TETRIS_DIRE_LEFT,   //!< 左移
    TETRIS_DIRE_RIGHT,  //!< 右移
    TETRIS_DIRE_DOWN,   //!< 下移
    TETRIS_DIRE_ROTATE, //!< 旋转
    TETRIS_DIRE_PROM_DOWN, //立即到底部
} TETRIS_DIRE_E;

#define BRICK_TYPE 7 // 一共7种类型的方块

#define BRICK_HEIGHT 4 // 一个brick由4*4的box组成
#define BRICK_WIDTH 4

#define MAP_WIDTH 10  // 地图宽
#define MAP_HEIGHT 20 // 地图高

//移动
extern bool tetris_move(TETRIS_DIRE_E direction);
//更新map数据
extern void tetris_sync(void);
extern void tetris_sync_all(void);
//当在地图中无法容纳新方块时, 游戏结束, 此函数返回
extern bool tetris_is_game_over(void);

// 初始化, 需要的回调函数说明:
// 在(x, y)画一个box, color为颜色, 注意0表示清除, 不表示任何颜色
// draw_box_to_map(uint8_t x, uint8_t y, uint8_t color)

// 函数须返回一个随机数, 产生新方块使用
// get_random(void)

// 当产生新方块后回调此函数, 参数为新方块的数据
// next_brick_info_map(uint8_t x, uint8_t y, uint8_t color)

// 当发生消行时回调此函数, 参数为消除的行数
// remove_line_num(uint8_t line)
extern void tetris_init(
    void (*draw_box_to_map)(uint8_t x, uint8_t y, uint8_t color),
    uint8_t (*get_random)(void),
    void (*next_brick_info_map)(uint8_t x, uint8_t y, uint8_t color),
    void (*remove_line_num)(uint8_t line));
#endif

/************* Copyright(C) 2013 - 2014 DevLabs **********END OF FILE**********/
