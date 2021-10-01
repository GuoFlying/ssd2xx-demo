#include "f_game_tetris.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define CANVAS_WIDTH 350
#define CANVAS_HEIGHT 350

/**
 * Create a transparent canvas with Chroma keying and indexed color format (palette).
 */
void lv_example_canvas_2(void)
{
    /*Create a button to better see the transparency*/
    lv_btn_create(lv_scr_act());

    /*Create a buffer for the canvas*/
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];

    /*Create a canvas and initialize its palette*/
    lv_obj_t *canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(canvas, 0, LV_COLOR_CHROMA_KEY);
    lv_canvas_set_palette(canvas, 1, lv_palette_main(LV_PALETTE_RED));

    /*Create colors with the indices of the palette*/
    lv_color_t c0;
    lv_color_t c1;

    c0.full = 0;
    c1.full = 1;

    /*Red background (There is no dedicated alpha channel in indexed images so LV_OPA_COVER is ignored)*/
    lv_canvas_fill_bg(canvas, c1, LV_OPA_COVER);

    /*Create hole on the canvas*/
    uint32_t x;
    uint32_t y;
    for (y = 10; y < 30; y++)
    {
        for (x = 5; x < 20; x++)
        {
            lv_canvas_set_px(canvas, x, y, c0);
        }
    }
}

typedef struct _lv_f_tetris_game
{
    lv_obj_t *bg;               /* 背景 */
    lv_obj_t *canvas_stage;     /* 游戏舞台 */
    lv_obj_t *label_best_score; /* 展示最高分数 */
    lv_obj_t *label_best_lines; /* 消除行数 */
    lv_obj_t *label_best_level; /* 等级 */

    lv_obj_t *canvas_preview; /* 下一个方块预览 */

} T_lv_f_tetris_game, *PT_lv_f_tetris_game;
static PT_lv_f_tetris_game g_pt_lv_f_tetis_game;

static void f_game_tetris_ctrl_event_cb(lv_event_t *e);
static void f_game_tetris_draw_box_to_map(uint8_t x, uint8_t y, uint8_t color);
static uint8_t f_game_tetris_get_random(void);
static void f_game_tetris_next_brick_info(uint8_t x, uint8_t y, uint8_t color);
static void f_game_tetris_remove_line_num(uint8_t line);
static void f_game_tetris_canvas_stage_init();
static void lv_task_f_game_tetris(struct _lv_timer_t *timer);
static void f_game_tetris_canvas_over_show();

void f_game_tetris()
{
    /* 申请内存 */
    g_pt_lv_f_tetis_game = (T_lv_f_tetris_game *)malloc(sizeof(T_lv_f_tetris_game));
    memset(g_pt_lv_f_tetis_game, 0, sizeof(T_lv_f_tetris_game));
    //初始化随机数
    srand(time(NULL));

    /* 初始化桌面背景 */
    g_pt_lv_f_tetis_game->bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(g_pt_lv_f_tetis_game->bg, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_y(g_pt_lv_f_tetis_game->bg, 0);
    /*标题*/
    lv_obj_t *label;
    label = lv_label_create(g_pt_lv_f_tetis_game->bg);
    lv_label_set_text(label, LV_SYMBOL_MINUS "  Tetris");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0000ff), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    /*俄罗斯方块舞台*/
    g_pt_lv_f_tetis_game->canvas_stage = lv_canvas_create(g_pt_lv_f_tetis_game->bg);
    lv_color_t *cbuf = (lv_color_t *)malloc(sizeof(lv_color_t) * LV_CANVAS_BUF_SIZE_TRUE_COLOR(F_TETRIS_STAGE_W, F_TETRIS_STAGE_H));
    lv_canvas_set_buffer(g_pt_lv_f_tetis_game->canvas_stage, cbuf, F_TETRIS_STAGE_W, F_TETRIS_STAGE_H, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align_to(g_pt_lv_f_tetis_game->canvas_stage, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    f_game_tetris_canvas_stage_init();

    /*下一个方块预览*/
    g_pt_lv_f_tetis_game->canvas_preview = lv_canvas_create(g_pt_lv_f_tetis_game->bg);
    cbuf = (lv_color_t *)malloc(sizeof(lv_color_t) * LV_CANVAS_BUF_SIZE_TRUE_COLOR(F_TETRIS_PREVIEW_NEX_W, F_TETRIS_PREVIEW_NEX_H));
    lv_canvas_set_buffer(g_pt_lv_f_tetis_game->canvas_preview, cbuf, F_TETRIS_PREVIEW_NEX_W, F_TETRIS_PREVIEW_NEX_H, LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(g_pt_lv_f_tetis_game->canvas_preview, lv_color_white(), LV_OPA_COVER);
    lv_obj_align_to(g_pt_lv_f_tetis_game->canvas_preview, g_pt_lv_f_tetis_game->canvas_stage, LV_ALIGN_OUT_LEFT_TOP, -30, 0);

    /*分数*/
    g_pt_lv_f_tetis_game->label_best_score = lv_label_create(g_pt_lv_f_tetis_game->bg);
    g_pt_lv_f_tetis_game->label_best_score->user_data = NULL;
    lv_label_set_text(g_pt_lv_f_tetis_game->label_best_score, "SCORE:  0");
    lv_obj_set_style_text_font(g_pt_lv_f_tetis_game->label_best_score, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(g_pt_lv_f_tetis_game->label_best_score, lv_color_hex(0xff0000), 0);
    lv_obj_align_to(g_pt_lv_f_tetis_game->label_best_score, g_pt_lv_f_tetis_game->canvas_stage, LV_ALIGN_OUT_RIGHT_TOP, 10, 20);

    g_pt_lv_f_tetis_game->label_best_lines = lv_label_create(g_pt_lv_f_tetis_game->bg);
    g_pt_lv_f_tetis_game->label_best_lines->user_data = NULL;
    lv_label_set_text(g_pt_lv_f_tetis_game->label_best_lines, "LINES:  0");
    lv_obj_set_style_text_font(g_pt_lv_f_tetis_game->label_best_lines, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(g_pt_lv_f_tetis_game->label_best_lines, lv_color_hex(0xff0000), 0);
    lv_obj_align_to(g_pt_lv_f_tetis_game->label_best_lines, g_pt_lv_f_tetis_game->label_best_score, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);

    g_pt_lv_f_tetis_game->label_best_level = lv_label_create(g_pt_lv_f_tetis_game->bg);
    g_pt_lv_f_tetis_game->label_best_level->user_data = NULL;
    lv_label_set_text(g_pt_lv_f_tetis_game->label_best_level, "LEVER:  0");
    lv_obj_set_style_text_font(g_pt_lv_f_tetis_game->label_best_level, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(g_pt_lv_f_tetis_game->label_best_level, lv_color_hex(0xff0000), 0);
    lv_obj_align_to(g_pt_lv_f_tetis_game->label_best_level, g_pt_lv_f_tetis_game->label_best_lines, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);

    /*按键*/
    static const char *btnm_map[] = {LV_SYMBOL_UP, "\n",
                                     LV_SYMBOL_LEFT, LV_SYMBOL_DOWN, LV_SYMBOL_RIGHT, ""};
    lv_obj_t *btnm = lv_btnmatrix_create(g_pt_lv_f_tetis_game->bg);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_set_size(btnm, F_TETRIS_BTN_W, F_TETRIS_BTN_H);
    lv_obj_set_style_border_opa(btnm, LV_OPA_0, 0);
    lv_obj_add_event_cb(btnm, f_game_tetris_ctrl_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_align_to(btnm, g_pt_lv_f_tetis_game->canvas_stage, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);

    static lv_style_t style_btn;
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_AMBER));
    lv_style_set_radius(&style_btn, 30);
    lv_obj_add_style(btnm, &style_btn, LV_PART_ITEMS);

    //初始化俄罗斯方块游戏
    tetris_init(f_game_tetris_draw_box_to_map, f_game_tetris_get_random, f_game_tetris_next_brick_info, f_game_tetris_remove_line_num);

    lv_timer_create(lv_task_f_game_tetris, F_ETRIS_BASE_MIN_SPEED, NULL); //小俄罗斯方块移动
}

static void f_game_tetris_draw_box_to_map(uint8_t x, uint8_t y, uint8_t color)
{
#define RECT_SIZE 2
    //printf("%d %d %d\r\n", x, y, color);
    static int is = 0;
    static lv_draw_rect_dsc_t rect_dsc[RECT_SIZE];
    if (is == 0)
    {
        is = 1;
        for (int i = 0; i < RECT_SIZE; i++)
        {
            lv_draw_rect_dsc_init(&rect_dsc[i]);
            rect_dsc[i].bg_opa = LV_OPA_COVER;
            rect_dsc[i].border_width = F_TETRIS_STAGE_LINE;
            rect_dsc[i].border_opa = LV_OPA_100;
            rect_dsc[i].border_color = lv_palette_main(LV_PALETTE_RED);
        }
        rect_dsc[0].border_opa = LV_OPA_0;
        rect_dsc[0].bg_color = lv_color_hex(0xb2d1ce); //没方格
        rect_dsc[1].bg_color = lv_color_hex(0xd67421);
    }
    lv_canvas_draw_rect(g_pt_lv_f_tetis_game->canvas_stage,
                        F_TETRIS_BOX_START_W(x), F_TETRIS_BOX_START_H(y),
                        F_TETRIS_STAGE_BOX_W, F_TETRIS_STAGE_BOX_H, &rect_dsc[color >= RECT_SIZE ? RECT_SIZE - 1 : color]);

#undef RECT_SIZE
}

static uint8_t f_game_tetris_get_random(void)
{
    uint8_t tmp = 0 - 1;
    return rand() % tmp;
}

static void f_game_tetris_next_brick_info(uint8_t x, uint8_t y, uint8_t color)
{
#define RECT_SIZE 2
    // printf("%d %d %d\r\n", x, y, color);
    static int is = 0;
    static lv_draw_rect_dsc_t rect_dsc[RECT_SIZE];
    if (is == 0)
    {
        is = 1;
        for (int i = 0; i < RECT_SIZE; i++)
        {
            lv_draw_rect_dsc_init(&rect_dsc[i]);
            rect_dsc[i].bg_opa = LV_OPA_COVER;
            rect_dsc[i].border_width = F_TETRIS_STAGE_LINE;
            rect_dsc[i].border_opa = LV_OPA_100;
            rect_dsc[i].border_color = lv_palette_main(LV_PALETTE_RED);
        }
        rect_dsc[0].border_opa = LV_OPA_0;
        rect_dsc[0].bg_color = lv_color_white(); //没方格
        rect_dsc[1].bg_color = lv_color_hex(0xd67421);
    }
    lv_canvas_draw_rect(g_pt_lv_f_tetis_game->canvas_preview,
                        F_TETRIS_PREVIEW_BOX_START_W(x), F_TETRIS_PREVIEW_BOX_START_H(y),
                        F_TETRIS_PREVIEW_BOX_W, F_TETRIS_PREVIEW_BOX_H, &rect_dsc[color >= RECT_SIZE ? RECT_SIZE - 1 : color]);

#undef RECT_SIZE
}

//放下一个方块就会回调一次
static void f_game_tetris_remove_line_num(uint8_t line)
{
    //LV_LOG_USER("%d",line);
    lv_obj_t *o_lines = g_pt_lv_f_tetis_game->label_best_lines;
    int *lines = (int *)&o_lines->user_data;
    *lines += line;
    lv_label_set_text_fmt(g_pt_lv_f_tetis_game->label_best_lines, "LINES:  %d", *lines);

    lv_obj_t *o_score = g_pt_lv_f_tetis_game->label_best_score;
    int *score = (int *)&o_score->user_data;
    *score += pow(3, line); //line== 0 表示放下一个方块，也要+1分
    lv_label_set_text_fmt(g_pt_lv_f_tetis_game->label_best_score, "SCORE:  %d", *score);

    lv_obj_t *o_lever = g_pt_lv_f_tetis_game->label_best_level;
    int *lever = (int *)&o_lever->user_data;
    *lever = *lines / 5;
    lv_label_set_text_fmt(g_pt_lv_f_tetis_game->label_best_level, "LEVER:  %d", *lever);
}

static void lv_task_f_game_tetris(struct _lv_timer_t *timer)
{
    static int lever_old = 0;
    tetris_move(TETRIS_DIRE_DOWN);
    tetris_sync();
    if (tetris_is_game_over())
    {
        lv_timer_del(timer);
        f_game_tetris_canvas_over_show();
    }

    int *lever = (int *)&g_pt_lv_f_tetis_game->label_best_level->user_data;
    if (lever_old != *lever)
    {
        lever_old = *lever;
        int speed = F_ETRIS_BASE_MIN_SPEED - F_ETRIS_BASE_STEP_SPEED * lever_old;
        lv_timer_set_period(timer, speed < F_ETRIS_BASE_MAX_SPEED ? F_ETRIS_BASE_MAX_SPEED : speed);
    }
}

static void f_game_tetris_ctrl_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code != LV_EVENT_VALUE_CHANGED)
    {
        return;
    }

    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    //LV_LOG_USER("%d was pressed\n", id);
    switch (id)
    {
    case 0:
        //LV_LOG_USER("UP\n");
        tetris_move(TETRIS_DIRE_ROTATE);
        break;
    case 1:
        //LV_LOG_USER("LEFT\n");
        tetris_move(TETRIS_DIRE_LEFT);
        break;
    case 2:
        //LV_LOG_USER("DOWN\n");
        tetris_move(TETRIS_DIRE_PROM_DOWN);
        break;
    case 3:
        //LV_LOG_USER("RIGHT\n");
        tetris_move(TETRIS_DIRE_RIGHT);
        break;
    default:
        break;
    }
    tetris_sync();
}

static void f_game_tetris_canvas_stage_init()
{

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_white();
    rect_dsc.border_width = F_TETRIS_STAGE_BORDER;
    rect_dsc.border_opa = LV_OPA_100;
    rect_dsc.border_color = lv_palette_main(LV_PALETTE_AMBER);
    lv_canvas_draw_rect(g_pt_lv_f_tetis_game->canvas_stage, 0, 0, F_TETRIS_STAGE_W, F_TETRIS_STAGE_H, &rect_dsc);
#if 0
    //画间隔线 方块自画即可
    lv_draw_line_dsc_t draw_line_dsc;
    lv_draw_line_dsc_init(&draw_line_dsc);
    draw_line_dsc.color = lv_color_hex(0xff00ff);
    draw_line_dsc.width = F_TETRIS_STAGE_LINE;
    draw_line_dsc.opa = LV_OPA_100;

    lv_point_t points[(F_TETRIS_STAGE_MAP_W - 1) * 2];
    int i;
    for (i = 0; i < (F_TETRIS_STAGE_MAP_W - 1) * 2; i += 2)
    {
        points[i + 1].x = points[i].x = F_TETRIS_STAGE_BORDER + (F_TETRIS_REAL_STAGE_W / F_TETRIS_STAGE_MAP_W) + i / 2 * F_TETRIS_REAL_STAGE_W / F_TETRIS_STAGE_MAP_W;
        points[i].y = 0 + F_TETRIS_STAGE_BORDER;
        points[i + 1].y = F_TETRIS_STAGE_H - F_TETRIS_STAGE_BORDER;
        lv_canvas_draw_line(g_pt_lv_f_tetis_game->canvas_stage, &points[i], 2, &draw_line_dsc);
    }
    //lv 源码全部画会交叉，为了先不改变源码，需要单独分开画线
    //lv_canvas_draw_line(g_pt_lv_f_tetis_game->canvas_stage,points,(F_TETRIS_STAGE_MAP_W-1)*2,&draw_line_dsc);

    lv_point_t points_h[(F_TETRIS_STAGE_MAP_H - 1) * 2];
    for (i = 0; i < (F_TETRIS_STAGE_MAP_H - 1) * 2; i += 2)
    {
        points_h[i + 1].y = points_h[i].y = F_TETRIS_STAGE_BORDER + (F_TETRIS_REAL_STAGE_H / F_TETRIS_STAGE_MAP_H) + i / 2 * F_TETRIS_REAL_STAGE_H / F_TETRIS_STAGE_MAP_H;
        points_h[i].x = 0 + F_TETRIS_STAGE_BORDER;
        points_h[i + 1].x = F_TETRIS_STAGE_W - F_TETRIS_STAGE_BORDER;
        lv_canvas_draw_line(g_pt_lv_f_tetis_game->canvas_stage, &points_h[i], 2, &draw_line_dsc);
    }
#endif
}

static void f_game_tetris_canvas_over_show()
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_white();
    rect_dsc.bg_opa = LV_OPA_70;
    lv_canvas_draw_rect(g_pt_lv_f_tetis_game->canvas_stage, 0, 0, F_TETRIS_STAGE_W, F_TETRIS_STAGE_H, &rect_dsc);

    lv_draw_label_dsc_t rect_dsc1;
    lv_draw_label_dsc_init(&rect_dsc1);
    rect_dsc1.color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc1.font = &lv_font_montserrat_36;
    rect_dsc1.align = LV_TEXT_ALIGN_CENTER;
    lv_canvas_draw_text(g_pt_lv_f_tetis_game->canvas_stage, 0, F_TETRIS_STAGE_H / 2, F_TETRIS_STAGE_W, &rect_dsc1, "GAME OVER!");
}