#include "f_game_2048.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static PT_lv_f_2048_game g_pt_lv_f_2048_game; // 数据结构体

/*函数声明*/
static lv_obj_t *add_title(lv_obj_t *obj, const char *txt);

static void event_handler_play_2048(lv_event_t *e);                                 // 触摸屏检测事件处理函数(上、下、左、右滑动
static void lv_f_game_2048_init(void);                                              // 界面初始化
static void lv_f_game_2048_init_board(lv_obj_t *parent, uint8_t board[SIZE][SIZE]); // 初始化画板，展示主界面效果
static void lv_f_game_2048_init_board_number(uint8_t board[SIZE][SIZE]);            // 初始化小方块数据
static void lv_f_game_2048_draw_board(lv_obj_t *parent, int number,
                                      uint16_t canvas_w, uint16_t canvas_h,
                                      uint16_t rect_dsc_x, uint16_t rect_dsc_y,
                                      lv_color_t rgb32);                                 // 填充绘制小方块
static void lv_f_game_2048_add_random(uint8_t board[SIZE][SIZE]);                        // 为二维数组初始化随机位置的随机数
static void lv_f_game_2048_rotate_board(uint8_t board[SIZE][SIZE]);                      // 旋转矩阵
static bool lv_f_game_2048_slide_array(uint8_t array[SIZE]);                             // 检查数组是否还有空间
static bool lv_f_game_2048_move_up(uint8_t board[SIZE][SIZE]);                           // 检查是否可以上滑动(触摸屏控制)
static bool lv_f_game_2048_move_left(uint8_t board[SIZE][SIZE]);                         // 检查是否可以左滑动(触摸屏控制)
static bool lv_f_game_2048_move_down(uint8_t board[SIZE][SIZE]);                         // 检查是否可以下滑动(触摸屏控制)
static bool lv_f_game_2048_move_right(uint8_t board[SIZE][SIZE]);                        // 检查是否可以右滑动(触摸屏控制)
static bool lv_f_game_2048_find_pair_down(uint8_t board[SIZE][SIZE]);                    // 检查是否可以右滑动
static bool lv_f_game_2048_game_ended(uint8_t board[SIZE][SIZE]);                        // 判断游戏是否应该结束
static char *lv_f_game_2048_int_to_string(int num, char *str);                           // 整型数值转为字符串
static lv_color_t lv_f_game_2048_num_to_color(uint32_t num);                             // 为数字设置对应的颜色
static uint8_t lv_f_game_2048_find_target(uint8_t array[SIZE], uint8_t x, uint8_t stop); // 检查是否应该停止滑动以避免双重合并
static uint8_t lv_f_game_2048_count_empty(uint8_t board[SIZE][SIZE]);                    // 查找当前二维数组是否有可以合并的数值(数值一样即可合并)

void lv_f_game_2048(void)
{
    /* 申请内存 */
    g_pt_lv_f_2048_game = (T_lv_f_2048_game *)malloc(sizeof(T_lv_f_2048_game));
    memset(g_pt_lv_f_2048_game, 0, sizeof(T_lv_f_2048_game));

    /* 初始化桌面背景 */
    g_pt_lv_f_2048_game->bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(g_pt_lv_f_2048_game->bg, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_y(g_pt_lv_f_2048_game->bg, 0);

    /* 初始化游戏舞台 */
    g_pt_lv_f_2048_game->canvas_stage = lv_obj_create(g_pt_lv_f_2048_game->bg);
    static lv_style_t stage_style;
    lv_style_init(&stage_style);
    lv_style_set_radius(&stage_style, 20);                         // 设置圆角
    lv_style_set_bg_color(&stage_style, LV_F_2048_GAME_BOX_COLOR); //设置颜色
    lv_obj_add_style(g_pt_lv_f_2048_game->canvas_stage, &stage_style, 0);

    lv_obj_set_size(g_pt_lv_f_2048_game->canvas_stage, LV_F_2048_GAME_BOX_W, LV_F_2048_GAME_BOX_H);
    lv_obj_align(g_pt_lv_f_2048_game->canvas_stage, LV_ALIGN_TOP_MID, 0, LV_F_2048_NUMBER_BOX_H);

    /* 初始化主界面 */
    g_pt_lv_f_2048_game->play_game = true;
    g_pt_lv_f_2048_game->current_score_value = 0;
    lv_f_game_2048_init();

    /* 分配屏幕触摸事件处理 */
    lv_obj_add_event_cb(lv_layer_top(), event_handler_play_2048, LV_EVENT_GESTURE, NULL);
    lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_GESTURE_BUBBLE); //清楚父类标志
    lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);        //控制信息可由外部设备输入

    /* 创建app标题 */
    add_title(g_pt_lv_f_2048_game->bg, "2048 GAME");
}

static lv_obj_t *add_title(lv_obj_t *obj, const char *txt)
{
#define TITLE_PAD (8)
    // 添加标题
    lv_obj_t *title = lv_label_create(obj);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x0000ff), 0);
    lv_label_set_text(title, txt);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, TITLE_PAD);
    return title;
}

/*
 *  函数名：   static void event_handler_play_2048(lv_obj_t * obj, lv_event_t event)
 *  输入参数： 触发事件的对象
 *  输入参数： 触发的事件类型
 *  返回值：   无
 *  函数作用： 触摸屏检测事件处理函数(上、下、左、右滑动)
*/
static void event_handler_play_2048(lv_event_t *e)
{
    if (g_pt_lv_f_2048_game->play_game == false)
    {
        return;
    }
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    //printf("GAME BEGIN!%d\n\r",dir);

    bool success = false;
    switch (dir)
    {
    case LV_DIR_LEFT:
        success = lv_f_game_2048_move_left(g_pt_lv_f_2048_game->game_board);
        break;
    case LV_DIR_RIGHT:
        success = lv_f_game_2048_move_right(g_pt_lv_f_2048_game->game_board);
        break;
    case LV_DIR_TOP:
        success = lv_f_game_2048_move_up(g_pt_lv_f_2048_game->game_board);
        break;
    case LV_DIR_BOTTOM:
        success = lv_f_game_2048_move_down(g_pt_lv_f_2048_game->game_board);
        break;
    default:
        success = false;
        break;
    }

    if (!success)
    {
        return;
    }

    lv_f_game_2048_init_board(g_pt_lv_f_2048_game->canvas_stage, g_pt_lv_f_2048_game->game_board);
    lv_f_game_2048_add_random(g_pt_lv_f_2048_game->game_board);
    lv_f_game_2048_init_board(g_pt_lv_f_2048_game->canvas_stage, g_pt_lv_f_2048_game->game_board);
    if (!lv_f_game_2048_game_ended(g_pt_lv_f_2048_game->game_board))
    {
        return;
    }

    //失败显示
    g_pt_lv_f_2048_game->play_game = false;

    lv_obj_t *title = lv_label_create(g_pt_lv_f_2048_game->bg);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xff0000), 0);
    lv_label_set_text(title, "GAME OVER!");
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -58);

    char buff[50] = {0};
    lv_obj_t *title1 = lv_label_create(g_pt_lv_f_2048_game->bg);
    lv_obj_set_style_text_font(title1, &lv_font_montserrat_40, 0);
    lv_obj_set_style_text_color(title1, lv_color_hex(0x00FF00), 0);
    sprintf(buff, "SCORE %d !", g_pt_lv_f_2048_game->current_score_value);
    lv_label_set_text(title1, buff);
    lv_obj_align_to(title1, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

/*
 *  函数名：   static void lv_f_stm32_tool_music_player_init(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 应用界面初始化
*/
static void lv_f_game_2048_init(void)
{
    /* 最高记录显示区域 */
    lv_obj_t *obj_best_source = lv_obj_create(g_pt_lv_f_2048_game->bg);
    lv_obj_set_size(obj_best_source, 120, 80);
    lv_obj_align_to(obj_best_source, g_pt_lv_f_2048_game->canvas_stage, LV_ALIGN_OUT_LEFT_TOP, 0, 40);

    /* 最高记录标题 */
    lv_obj_t *best_source_tip_text = lv_label_create(obj_best_source); /* 创建标签 */
    lv_label_set_text(best_source_tip_text, "BEST");
    lv_obj_align_to(best_source_tip_text, NULL, LV_ALIGN_TOP_MID, 0, 0); /* 居中 */

    /* 最高记录分数 */
    g_pt_lv_f_2048_game->label_best_score = lv_label_create(obj_best_source); /* 创建标签 */
    lv_label_set_text(g_pt_lv_f_2048_game->label_best_score, "40960");
    lv_obj_align_to(g_pt_lv_f_2048_game->label_best_score, NULL, LV_ALIGN_BOTTOM_MID, 0, 0); /* 居中 */

    /* 当前分数显示区域 */
    lv_obj_t *obj_current_source = lv_obj_create(g_pt_lv_f_2048_game->bg);
    lv_obj_set_size(obj_current_source, 120, 80);
    lv_obj_align_to(obj_current_source, g_pt_lv_f_2048_game->canvas_stage, LV_ALIGN_OUT_RIGHT_TOP, 0, 40);

    /* 当前分提示 */
    lv_obj_t *current_source_tip_text = lv_label_create(obj_current_source); /* 创建标签 */
    lv_label_set_text(current_source_tip_text, "SCORE");
    lv_obj_align_to(current_source_tip_text, NULL, LV_ALIGN_TOP_MID, 0, 0); /* 居中 */

    /* 当前分数 */
    g_pt_lv_f_2048_game->label_current_score = lv_label_create(obj_current_source); /* 创建标签 */
    lv_label_set_text(g_pt_lv_f_2048_game->label_current_score, "0");
    lv_obj_align_to(g_pt_lv_f_2048_game->label_current_score, NULL, LV_ALIGN_BOTTOM_MID, 0, 0); /* 当前画布居中 */

    /* 初始化游戏数字 */
    lv_f_game_2048_init_board_number(g_pt_lv_f_2048_game->game_board);

    /* 初始化游戏舞台中的每个方格块 */
    lv_f_game_2048_init_board(g_pt_lv_f_2048_game->canvas_stage, g_pt_lv_f_2048_game->game_board);
}

/*
 *  函数名：   static void lv_f_game_2048_init_board(lv_obj_t * parent, uint8_t board[SIZE][SIZE])
 *  输入参数： 父对象
 *  返回值：   需要操作的数组
 *  函数作用： 初始化画板，展示主界面效果
*/
static void lv_f_game_2048_init_board(lv_obj_t *parent, uint8_t board[SIZE][SIZE])
{
    /* 清除之前所有的子对象 */
    lv_obj_clean(parent);

    /* 当前分数 */
    char str_current_score[8] = {0};
    lv_label_set_text(g_pt_lv_f_2048_game->label_current_score, lv_f_game_2048_int_to_string(g_pt_lv_f_2048_game->current_score_value, str_current_score));

    for (int y = 0; y < SIZE; y++)
    {
        for (int x = 0; x < SIZE; x++)
        {
            if (board[x][y] != 0)
            {
                lv_f_game_2048_draw_board(parent, (int)1 << board[x][y],
                                          LV_F_2048_NUMBER_BOX_W, LV_F_2048_NUMBER_BOX_H,
                                          (8 * (y + 1)) + (LV_F_2048_NUMBER_BOX_W * (y + 1 - 1)), (8 * (x + 1)) + (LV_F_2048_NUMBER_BOX_W * (x + 1 - 1)), lv_f_game_2048_num_to_color((uint32_t)1 << board[x][y]));
            }
            else
            {
                lv_f_game_2048_draw_board(parent, -1,
                                          LV_F_2048_NUMBER_BOX_W, LV_F_2048_NUMBER_BOX_H,
                                          (8 * (y + 1)) + (LV_F_2048_NUMBER_BOX_W * (y + 1 - 1)), (8 * (x + 1)) + (LV_F_2048_NUMBER_BOX_W * (x + 1 - 1)), LV_F_2048_NUMBER_BOX_COLOR);
            }
        }
    }
}

/*
 *  函数名：   static void lv_f_game_2048_draw_board(lv_obj_t * parent, int number, \
														 uint16_t canvas_w, uint16_t canvas_h, \
														 uint16_t rect_dsc_x, uint16_t rect_dsc_y, \
														 lv_color_t rgb32)
 *  输入参数： 绘制所依托的父对象
 *  输入参数： 展示的数值
 *  输入参数： 宽度
 *  输入参数： 高度
 *  输入参数： x轴上的偏移值
 *  输入参数： y轴上的偏移值
 *  输入参数： 方块背景颜色
 *  返回值：   无
 *  函数作用： 填充绘制小方块
*/
static void lv_f_game_2048_draw_board(lv_obj_t *parent, int number,
                                      uint16_t canvas_w, uint16_t canvas_h,
                                      uint16_t rect_dsc_x, uint16_t rect_dsc_y,
                                      lv_color_t rgb32)
{

    /* 创建方格块 */
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_size(obj, canvas_w, canvas_h);
    lv_obj_set_style_bg_color(obj, rgb32, 0);
    lv_obj_set_style_radius(obj, 3, 0);
    lv_obj_set_style_border_opa(obj, LV_OPA_0, 0);
    if(number >= 1000){
        lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, 0);
    }
    else if(number >= 100){
        lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
    }else{
        lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, 0);
    }
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, rect_dsc_y, rect_dsc_x);

    /* 数字 */
    lv_obj_t *icon = lv_label_create(obj); /* 创建标签 */
    //sprintf(tmp_char, "%d", board[x][y]);
    if (number != -1)
    {
        char tmp_char[8];
        snprintf(tmp_char, 8, "%u", number);
        lv_label_set_text(icon, tmp_char);
        
    }
    else
    {
        lv_label_set_text(icon, "");
    }

    lv_obj_center(icon); /* 当前画布居中 */
}

/*
 *  函数名：   static char* lv_f_game_2048_int_to_string(int num, char *str)
 *  输入参数： 需要转换的整型数值
 *  返回值：   转换后的字符串
 *  函数作用： 整型数值转为字符串
*/
static char *lv_f_game_2048_int_to_string(int num, char *str)
{
    int i = 0;   //指示填充str
    if (num < 0) //如果num为负数，将num变正
    {
        num = -num;
        str[i++] = '-';
    }
    //转换
    do
    {
        str[i++] = num % 10 + 48; //取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
        num /= 10;                //去掉最低位
    } while (num);                //num不为0继续循环

    str[i] = '\0';

    //确定开始调整的位置
    int j = 0;
    if (str[0] == '-') //如果有负号，负号不用调整
    {
        j = 1; //从第二位开始调整
        ++i;   //由于有负号，所以交换的对称轴也要后移1位
    }
    //对称交换
    for (; j < i / 2; j++)
    {
        //对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b;
        str[j] = str[j] + str[i - 1 - j];
        str[i - 1 - j] = str[j] - str[i - 1 - j];
        str[j] = str[j] - str[i - 1 - j];
    }

    return str; //返回转换后的值
}

/*
 *  函数名：   static void lv_f_game_2048_init_board_number(uint8_t board[SIZE][SIZE])
 *  输入参数： 需要进行初始化的数组
 *  返回值：   无
 *  函数作用： 为二维数组初始化数值(全写0)
*/
static void lv_f_game_2048_init_board_number(uint8_t board[SIZE][SIZE])
{
    uint8_t x, y;
    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE; y++)
        {
            board[x][y] = 0;
        }
    }

    /* 初始化两个随机位置的随机数 */
    lv_f_game_2048_add_random(board);
    lv_f_game_2048_add_random(board);

    g_pt_lv_f_2048_game->current_score_value = 0;
}

/*
 *  函数名：   static void lv_f_game_2048_add_random(uint8_t board[SIZE][SIZE])
 *  输入参数： 需要初始化随机位置的随机数的数组
 *  返回值：   无
 *  函数作用： 为二维数组初始化随机位置的随机数
*/
static void lv_f_game_2048_add_random(uint8_t board[SIZE][SIZE])
{
    static bool initialized = false;
    uint8_t x, y;
    uint8_t r, len = 0;
    uint8_t n, list[SIZE * SIZE][2];

    if (!initialized)
    {
        srand(time(NULL));
        initialized = true;
    }

    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE; y++)
        {
            if (board[x][y] == 0)
            {
                list[len][0] = x;
                list[len][1] = y;
                len++;
            }
        }
    }

    if (len > 0)
    {
        r = rand() % len;
        x = list[r][0];
        y = list[r][1];
        n = (rand() % 10) / 9 + 1;
        board[x][y] = n;
    }
}

/*
 *  函数名：   static lv_color_t lv_f_game_2048_num_to_color(uint32_t num)
 *  输入参数： 需要设置颜色的数值
 *  返回值：   颜色值
 *  函数作用： 为数字设置对应的颜色
*/
static lv_color_t lv_f_game_2048_num_to_color(uint32_t num)
{
    switch (num)
    {
    case 2:
        return LV_F_2048_NUMBER_2_COLOR;
    case 4:
        return LV_F_2048_NUMBER_4_COLOR;
    case 8:
        return LV_F_2048_NUMBER_8_COLOR;
    case 16:
        return LV_F_2048_NUMBER_16_COLOR;
    case 32:
        return LV_F_2048_NUMBER_32_COLOR;
    case 64:
        return LV_F_2048_NUMBER_64_COLOR;
    case 128:
        return LV_F_2048_NUMBER_128_COLOR;
    case 256:
        return LV_F_2048_NUMBER_256_COLOR;
    case 512:
        return LV_F_2048_NUMBER_512_COLOR;
    case 1024:
        return LV_F_2048_NUMBER_1024_COLOR;
    case 2048:
        return LV_F_2048_NUMBER_2048_COLOR;
    default:
        return LV_F_2048_NUMBER_2048_COLOR;
    }
}

/*
 *  函数名：   static uint8_t lv_f_game_2048_find_target(uint8_t array[SIZE],uint8_t x,uint8_t stop) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   目标位置
 *  函数作用： 检查是否应该停止滑动以避免双重合并
*/
static uint8_t lv_f_game_2048_find_target(uint8_t array[SIZE], uint8_t x, uint8_t stop)
{
    uint8_t t;
    // if the position is already on the first, don't evaluate
    if (x == 0)
    {
        return x;
    }
    for (t = (x - 1);; t--)
    {
        if (array[t] != 0)
        {
            if (array[t] != array[x])
            {
                // merge is not possible, take next position
                return (t + 1);
            }
            return t;
        }
        else
        {
            // we should not slide further, return this one
            if (t == stop)
            {
                return t;
            }
        }
    }
    // we did not find a
    return x;
}

/*
 *  函数名：   static bool lv_f_game_2048_slide_array(uint8_t array[SIZE]) 
 *  输入参数： 需要检查的数组
 *  返回值：   true - 尚有空间； false - 无多余空间
 *  函数作用： 检查数组是否还有空间
*/
static bool lv_f_game_2048_slide_array(uint8_t array[SIZE])
{
    bool success = false;
    uint8_t x, t, stop = 0;

    for (x = 0; x < SIZE; x++)
    {
        if (array[x] != 0)
        {
            t = lv_f_game_2048_find_target(array, x, stop);
            // if target is not original position, then move or merge
            if (t != x)
            {
                // if target is zero, this is a move
                if (array[t] == 0)
                {
                    array[t] = array[x];
                }
                else if (array[t] == array[x])
                {
                    // merge (increase power of two)
                    array[t]++;
                    // increase score
                    g_pt_lv_f_2048_game->current_score_value += (uint32_t)1 << array[t];
                    // set stop to avoid double merge
                    stop = t + 1;
                }
                array[x] = 0;
                success = true;
            }
        }
    }
    return success;
}

/*
 *  函数名：   static bool lv_f_game_2048_rotate_board(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要旋转的矩阵
 *  返回值：   无
 *  函数作用： 旋转矩阵
*/
static void lv_f_game_2048_rotate_board(uint8_t board[SIZE][SIZE])
{
    uint8_t i, j, n = SIZE;
    uint8_t tmp;
    for (i = 0; i < n / 2; i++)
    {
        for (j = i; j < n - i - 1; j++)
        {
            tmp = board[i][j];
            board[i][j] = board[j][n - i - 1];
            board[j][n - i - 1] = board[n - i - 1][n - j - 1];
            board[n - i - 1][n - j - 1] = board[n - j - 1][i];
            board[n - j - 1][i] = tmp;
        }
    }
}

/*
 *  函数名：   static bool lv_f_game_2048_move_up(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以上滑动
*/
static bool lv_f_game_2048_move_up(uint8_t board[SIZE][SIZE])
{
    printf("%s\r\n", __func__);

    bool success = false;
    uint8_t x;
    for (x = 0; x < SIZE; x++)
    {
        success |= lv_f_game_2048_slide_array(board[x]);
    }
    return success;
}

/*
 *  函数名：   static bool lv_f_game_2048_move_left(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以上滑动
*/
static bool lv_f_game_2048_move_left(uint8_t board[SIZE][SIZE])
{
    printf("%s\r\n", __func__);
    bool success;
    lv_f_game_2048_rotate_board(board);
    success = lv_f_game_2048_move_up(board);
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    return success;
}

/*
 *  函数名：   static bool lv_f_game_2048_move_down(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以下滑动
*/
static bool lv_f_game_2048_move_down(uint8_t board[SIZE][SIZE])
{
    printf("%s\r\n", __func__);

    bool success;
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    success = lv_f_game_2048_move_up(board);
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    return success;
}

/*
 *  函数名：   static bool lv_f_game_2048_move_right(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要检查的二维数组
 *  返回值：   true - 可以滑动； false - 不可以滑动
 *  函数作用： 检查是否可以右滑动
*/
static bool lv_f_game_2048_move_right(uint8_t board[SIZE][SIZE])
{
    printf("%s\r\n", __func__);

    bool success = false;
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    success = lv_f_game_2048_move_up(board);
    lv_f_game_2048_rotate_board(board);
    return success;
}

/*
 *  函数名：   static uint8_t lv_f_game_2048_count_empty(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要查找的二维数组
 *  返回值：   true - 有可以合并的数值； false - 没有可以合并的数值
 *  函数作用： 查找当前二维数组是否有可以合并的数值(数值一样即可合并)
*/
static bool lv_f_game_2048_find_pair_down(uint8_t board[SIZE][SIZE])
{
    printf("%s\r\n", __func__);

    bool success = false;
    uint8_t x, y;
    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE - 1; y++)
        {
            if (board[x][y] == board[x][y + 1])
                return true;
        }
    }
    return success;
}

/*
 *  函数名：   static uint8_t lv_f_game_2048_count_empty(uint8_t board[SIZE][SIZE]) 
 *  输入参数： 需要统计的二维数组
 *  返回值：   空闲的方格个数
 *  函数作用： 统计空闲的方格个数
*/
static uint8_t lv_f_game_2048_count_empty(uint8_t board[SIZE][SIZE])
{
    uint8_t x, y;
    uint8_t count = 0;
    for (x = 0; x < SIZE; x++)
    {
        for (y = 0; y < SIZE; y++)
        {
            if (board[x][y] == 0)
            {
                count++;
            }
        }
    }
    return count;
}

// 游戏结束

/*
 *  函数名：   static bool lv_f_game_2048_game_ended(uint8_t board[SIZE][SIZE])
 *  输入参数： 无
 *  返回值：   true - 游戏结束， false - 可继续游戏
 *  函数作用： 判断游戏是否应该结束
*/
static bool lv_f_game_2048_game_ended(uint8_t board[SIZE][SIZE])
{
    bool ended = true;
    if (lv_f_game_2048_count_empty(board) > 0)
        return false;
    if (lv_f_game_2048_find_pair_down(board))
        return false;
    lv_f_game_2048_rotate_board(board);
    if (lv_f_game_2048_find_pair_down(board))
        ended = false;
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    lv_f_game_2048_rotate_board(board);
    return ended;
}
