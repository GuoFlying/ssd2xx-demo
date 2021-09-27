#include <stdio.h>
#include <stdlib.h>
#include "f_game_snake.h"

static PT_lv_f_snake g_pt_lv_f_snake;  	// 数据结构体
static link_snake *spriteSnake;              		// 链表头(蛇头)

static lv_obj_t *add_title(lv_obj_t *obj, const char *txt);
static void lv_f_game_snake_init(void);							// 初始化界面
static link_snake *lv_f_game_snake_initLink(void);				// 初始化链表
static void lv_f_game_snake_clear_list(link_snake* ppHeadNode);	// 清空链表，释放结点内存，将链表重置为空表
static void lv_f_game_snake_linkAddNode(void);					// 在链表尾部插入新节点(增长小蛇身体)
static void lv_f_game_snake_update_snake_data(void); 			// 更新小蛇数据结构
static void event_handler_snake_gesture_cb(lv_event_t *e);	// 触摸屏检测处理事件(触摸屏控制游戏)
static void event_handler_back_to_home(lv_obj_t * obj, lv_event_t event);	// 返回桌面事件处理函数
static void lv_task_f_game_snake(struct _lv_timer_t * timer); //不断移动小蛇
/*
 *  函数名：   void lv_f_game_snake(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 应用初始化入口
*/
void lv_f_game_snake(void)
{
	g_pt_lv_f_snake = (T_lv_f_snake *)malloc(sizeof(T_lv_f_snake));   // 申请内存
	g_pt_lv_f_snake->gesture = 0; 

	g_pt_lv_f_snake->bg = lv_obj_create(lv_scr_act());
	lv_obj_set_size(g_pt_lv_f_snake->bg, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_y(g_pt_lv_f_snake->bg, 0);

    lv_f_game_snake_init();

    /* 创建任务检测按键输入与游戏动画 */
	lv_obj_add_event_cb(lv_layer_top(), event_handler_snake_gesture_cb, LV_EVENT_GESTURE, NULL);
	lv_obj_add_event_cb(lv_layer_top(), event_handler_snake_gesture_cb, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_GESTURE_BUBBLE); //清楚父类标志
    lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);        //控制信息可由外部设备输入

    lv_timer_create(lv_task_f_game_snake,150,NULL);//小蛇移动

    add_title(g_pt_lv_f_snake->bg, "SNAKE");							// 标题
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
 *  函数名：   static void lv_f_game_snake_init(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 应用界面初始化
*/
static void lv_f_game_snake_init(void)
{
    spriteSnake = lv_f_game_snake_initLink();

    link_snake* head = NULL;
    head = spriteSnake;         //将temp指针重新指向头结点
    int i = 0;
    lv_coord_t init_x = (rand()%(LV_HOR_RES));
    lv_coord_t init_y = (rand()%(LV_VER_RES));
    while (head->next)
    {
        head = head->next;
        head->obj = lv_obj_create(g_pt_lv_f_snake->bg);
        if (i == 0)
        {
            lv_obj_set_style_bg_color(head->obj,LV_F_GAME_SNAKE_HEAD_COLOR,0);//设置颜色
        }
        else{
            lv_obj_set_style_bg_color(head->obj,LV_F_GAME_SNAKE_BODY_COLOR,0);//设置颜色
        }
        lv_obj_set_style_radius(head->obj,LV_F_GAME_SNAKE_RADIUS,0); // 设置圆角
        lv_obj_set_size(head->obj, LV_F_GAME_SNAKE_SIZE, LV_F_GAME_SNAKE_SIZE);
        lv_obj_set_pos(head->obj, init_x+(i * LV_F_GAME_SNAKE_SIZE), init_y);
        head->x = lv_obj_get_x(head->obj);
        head->y = lv_obj_get_y(head->obj);
        i++;
    }

    // 初始化食物
    g_pt_lv_f_snake->obj_food = lv_obj_create(g_pt_lv_f_snake->bg);
    lv_obj_set_size(g_pt_lv_f_snake->obj_food, LV_F_GAME_FOOD_SIZE, LV_F_GAME_FOOD_SIZE);
    lv_obj_set_style_bg_color(g_pt_lv_f_snake->obj_food,  LV_F_GAME_FOOD_COLOR,0); //设置颜色
    lv_obj_set_style_radius(g_pt_lv_f_snake->obj_food,LV_F_GAME_FOOD_RADIUS,0);	// 设置圆角
    lv_obj_set_pos(g_pt_lv_f_snake->obj_food, (rand()%(LV_F_GAME_FOOD_MAX_HOR)), (rand()%(LV_F_GAME_FOOD_MAX_VER)));
	
	g_pt_lv_f_snake->sroce = 3;
    g_pt_lv_f_snake->len = 3;
    g_pt_lv_f_snake->label_info = lv_label_create(g_pt_lv_f_snake->bg);
	lv_obj_set_style_text_font(g_pt_lv_f_snake->label_info, &lv_font_montserrat_14,0);  		// 设置字体格式
	lv_label_set_text_fmt(g_pt_lv_f_snake->label_info, "LEN: %d\nSROCE: %d", g_pt_lv_f_snake->len, g_pt_lv_f_snake->sroce);  // 展示游戏信息
	lv_obj_align(g_pt_lv_f_snake->label_info, LV_ALIGN_TOP_LEFT, 0, LV_F_GAME_SNAKE_TITLE_SPACE);
}

// 
/*
 *  函数名：   static link_snake* lv_f_game_snake_initLink(void)
 *  输入参数： 无
 *  返回值：   初始化后的链表
 *  函数作用： 初始化链表(蛇)
*/
static link_snake* lv_f_game_snake_initLink(void)
{
    link_snake* head = (link_snake*)malloc(sizeof(link_snake));		// 创建链表第一个结点（首元结点）
    head->prior = NULL;
    head->next = NULL;
    head->obj = NULL;
    head->x = 0;
    head->y = 0;
    link_snake * list = head;   //声明一个指针指向头结点，方便后期向链表中添加新创建的节点

    // 生成链表(初始化蛇身)
    for (int i = 0; i < LV_F_GAME_SNAKE_INIT_LINE; i++) {
        link_snake *body = (link_snake*)malloc(sizeof(link_snake));
        body->prior = NULL;
        body->next = NULL;
        body->obj = NULL;
        body->x = i;
        body->y = i;

        //新节点与链表最后一个节点建立关系
        list->next = body;
        body->prior = list;
        //list永远指向链表中最后一个节点
        list = list->next;
    }
    return head;
}



/*
 *  函数名：   static void lv_f_game_snake_linkAddNode(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 在链表尾部插入新节点(增长蛇身)
*/
static void lv_f_game_snake_linkAddNode(void)
{
    link_snake *temp_list;

    // 初始化新节点
    link_snake *list_new = (link_snake*)malloc(sizeof(link_snake));
    list_new->prior = NULL;
    list_new->next = NULL;
    list_new->x = 0;
    list_new->y = 0;
    list_new->obj = lv_obj_create(g_pt_lv_f_snake->bg);
    lv_obj_set_style_bg_color(list_new->obj, LV_F_GAME_SNAKE_BODY_COLOR,0); //设置颜色
    lv_obj_set_style_radius(list_new->obj, LV_F_GAME_SNAKE_RADIUS,0);  // 设置圆角
    lv_obj_set_size(list_new->obj, LV_F_GAME_SNAKE_SIZE, LV_F_GAME_SNAKE_SIZE);

    // 检查头节点
    if(spriteSnake == NULL)
    {
        spriteSnake = list_new;
    }
    else
    {
        temp_list = spriteSnake;
        while(temp_list->next)  // 定位到链表尾部
        {
            temp_list = temp_list->next;
        }
        temp_list->next = list_new;
        list_new->prior = temp_list;
    }
}


/*
 *  函数名：   static void lv_f_game_snake_clear_list(link_snake* ppHeadNode)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 清空链表，释放结点内存，将链表重置为空表
*/
static void lv_f_game_snake_clear_list(link_snake* ppHeadNode)
{
    link_snake* pListNodeTmp = NULL;
    if((ppHeadNode) == NULL)
    {
        printf("The list is empty, no need to clear.\n");
        return;
    }

    //bool is_head = true;
    // 循环释放链表中的结点所占内存，
    while((ppHeadNode)->next != NULL)
    {
        pListNodeTmp = (ppHeadNode)->next;
		if (ppHeadNode->obj != NULL)	lv_obj_clean(ppHeadNode->obj);  // 删除蛇身节点
        free((ppHeadNode));
        (ppHeadNode) = pListNodeTmp;
    }

    // 清除最后一个结点
    if((ppHeadNode) != NULL)
    {
		if (ppHeadNode->obj != NULL)	lv_obj_clean(ppHeadNode->obj);  // 删除蛇身节点
        free((ppHeadNode));
        (ppHeadNode) = NULL;
    }

    printf("The list is cleared.\n");
}

/*
 *  函数名：   static void event_handler_snake_gesture_cb(lv_event_t *e)
 *  输入参数： 触发事件的对象
 *  输入参数： 触发地事件类型
 *  返回值：   无
 *  函数作用： 触摸屏检测事件处理，控制小蛇移动
*/
static void event_handler_snake_gesture_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_GESTURE){
         lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        //printf("GAME BEGIN!%d\n\r",dir);

        bool success = false;
        switch (dir)
        {
        case LV_DIR_LEFT:
            g_pt_lv_f_snake->gesture |= 8;
            // printf("LV_DIR_LEFT\r\n");
            break;
        case LV_DIR_RIGHT:
            g_pt_lv_f_snake->gesture |= 4;
            // printf("LV_DIR_RIGHT\r\n");
            break;
        case LV_DIR_TOP:
            g_pt_lv_f_snake->gesture |= 1;
            // printf("LV_DIR_TOP\r\n");
            break;
        case LV_DIR_BOTTOM:
            g_pt_lv_f_snake->gesture |= 2;
            // printf("LV_DIR_BOTTOM\r\n");
            break;
        default:
            break;
        }
        lv_f_game_snake_update_snake_data();
    }else if (code == LV_EVENT_LONG_PRESSED_REPEAT)   // 当长按时加速前进
    {
        lv_f_game_snake_update_snake_data();
        // printf("LV_EVENT_LONG_PRESSED\r\n");
    }

}

/*
 *  函数名：   static void lv_f_game_snake_update_snake_data(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 更新小蛇数据结构
*/
static void lv_f_game_snake_update_snake_data(void)
{
    static lv_coord_t x = LV_F_GAME_SNAKE_SPEED, y = 0;  // 默认往右边走
    if (((g_pt_lv_f_snake->gesture) & 1) == 1)         // 上
    {
        y = -LV_F_GAME_SNAKE_SPEED;
        x = 0;
    }
    else if (((g_pt_lv_f_snake->gesture) & 2) == 2)   // 下
    {
        y = LV_F_GAME_SNAKE_SPEED;
        x = 0;
    }
    else if (((g_pt_lv_f_snake->gesture) & 4) == 4)   // 左
    {
        y = 0;
        x = LV_F_GAME_SNAKE_SPEED;
    }
    else if (((g_pt_lv_f_snake->gesture) & 8) == 8)   // 右
    {
        y = 0;
        x = -LV_F_GAME_SNAKE_SPEED;
    }

    g_pt_lv_f_snake->gesture = 0;  // 清空标志位

    link_snake* temp_list = NULL;
    temp_list = spriteSnake;    // 将 temp_list 指针重新指向头结点
    while (temp_list->next)     // 定位到链表尾部
    {
        temp_list = temp_list->next;
    }
    while (temp_list->prior->prior)  // 首节点不加入
    {
        // 从后往前迭代更新坐标数据
        temp_list->x = temp_list->prior->x;
        temp_list->y = temp_list->prior->y;
        lv_obj_set_pos(temp_list->obj, temp_list->x, temp_list->y);
        temp_list = temp_list->prior;
    }
    // 处理首节点
    temp_list->x = lv_obj_get_x(temp_list->obj) + x;
    temp_list->y = lv_obj_get_y(temp_list->obj) + y;
    
    //超出边界回头再来
    temp_list->y =temp_list->y>LV_VER_RES?0:temp_list->y;
    temp_list->y = temp_list->y<0?LV_VER_RES:temp_list->y;
    temp_list->x = temp_list->x>LV_HOR_RES?0:temp_list->x;
    temp_list->x = temp_list->x<0?LV_HOR_RES:temp_list->x;

    lv_obj_set_pos(temp_list->obj, temp_list->x, temp_list->y);
}

/*
 *  函数名：   static void lv_task_f_game_snake(struct _lv_timer_t * timer)
 *  输入参数： 任务描述符
 *  返回值：   无
 *  函数作用： 不断地移动小蛇
*/

static void lv_task_f_game_snake(struct _lv_timer_t * timer)
{

    lv_coord_t x = spriteSnake->next->x - lv_obj_get_x(g_pt_lv_f_snake->obj_food);   // 获取 x 轴的差异
    lv_coord_t y = spriteSnake->next->y - lv_obj_get_y(g_pt_lv_f_snake->obj_food);   // 获取 y 轴的差异
    if(((x >= -LV_F_GAME_FOOD_CHECK_SPACE)&&(x <= LV_F_GAME_FOOD_CHECK_SPACE)) && ((y >= -LV_F_GAME_FOOD_CHECK_SPACE)&&(y <= LV_F_GAME_FOOD_CHECK_SPACE)))
    {
        lv_f_game_snake_linkAddNode();
        g_pt_lv_f_snake->len += 1;
        g_pt_lv_f_snake->sroce += 1;
		
        lv_obj_set_pos(g_pt_lv_f_snake->obj_food, rand()%(LV_F_GAME_FOOD_MAX_HOR), rand()%(LV_F_GAME_FOOD_MAX_VER));
		lv_label_set_text_fmt(g_pt_lv_f_snake->label_info, "LEN: %u\nSROCE: %u", g_pt_lv_f_snake->len, g_pt_lv_f_snake->sroce);  // 展示游戏信息
    }
    lv_f_game_snake_update_snake_data();
}