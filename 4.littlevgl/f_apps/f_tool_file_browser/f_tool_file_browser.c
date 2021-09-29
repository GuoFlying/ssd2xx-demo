#include <stdlib.h>
#include <stdio.h>
#include "f_tool_file_browser.h"
#include "file_rover.h"

#define LV_F_FILE_BROWSER_MAX_BUF (128) // 目录路径最大长度

typedef struct _lv_f_file_browser
{
	lv_obj_t *bg_file_browser;				  // 背景
	file_exec_func_list_t *exec_list;
	int  exec_count;
	f_row_list_t rows;
	char dir_path[FILE_PATH_MAX];
	lv_obj_t *list;              
} T_lv_f_file_browser, *PT_lv_f_file_browser;

static PT_lv_f_file_browser g_pt_lv_f_file_browser; // 数据结构体
static bool lv_f_tool_file_browser_init(void);		// 界面初始化
static void lv_f_tool_file_update_list(lv_obj_t * list1);
static void lv_f_tool_file_clear_list(lv_obj_t * list1);

/*
 *  函数名：   void lv_f_tool_file_browser(void)
 *  输入参数： 无
 *  返回值：   无
 *  函数作用： 应用初始化入口
*/
void lv_f_tool_file_browser(file_exec_func_list_t *exec_list, int exec_count)
{
	g_pt_lv_f_file_browser = (T_lv_f_file_browser *)malloc(sizeof(T_lv_f_file_browser)); // 申请内存
	memset(g_pt_lv_f_file_browser,0,sizeof(T_lv_f_file_browser));
	g_pt_lv_f_file_browser->exec_list = exec_list;
	g_pt_lv_f_file_browser->exec_count = exec_count;

	g_pt_lv_f_file_browser->bg_file_browser = lv_obj_create(lv_scr_act());
	lv_obj_set_size(g_pt_lv_f_file_browser->bg_file_browser, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_y(g_pt_lv_f_file_browser->bg_file_browser, 0);
	
	lv_obj_t *label;
	label = lv_label_create(g_pt_lv_f_file_browser->bg_file_browser);
	lv_label_set_text(label, LV_SYMBOL_DIRECTORY "  File Browser");
	lv_obj_set_style_text_font(label, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0000ff), 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

	/*Create a list*/
    g_pt_lv_f_file_browser->list = lv_list_create(g_pt_lv_f_file_browser->bg_file_browser);
    lv_obj_set_size(g_pt_lv_f_file_browser->list, LV_HOR_RES-50, LV_VER_RES-100);
    lv_obj_align_to(g_pt_lv_f_file_browser->list,label,LV_ALIGN_OUT_BOTTOM_MID,0,20);
	lv_obj_set_style_text_font(g_pt_lv_f_file_browser->list, &lv_font_montserrat_16, 0);
	lv_obj_set_style_pad_row(g_pt_lv_f_file_browser->list, 5, 0);

	lv_f_tool_file_browser_init();
}


static void lv_f_tool_file_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code != LV_EVENT_CLICKED) {
		return;
	}
	char *text = (char *)lv_list_get_btn_text(g_pt_lv_f_file_browser->list, obj);
	LV_LOG_USER("Clicked: %s", text);

	char *tail_name;
    tail_name = strrchr(text, '.') + 1;
	if(!tail_name){
		return;
	}
	
	for(int i = 0;i<g_pt_lv_f_file_browser->exec_count;i++){
		if(0 != strcmp(tail_name,g_pt_lv_f_file_browser->exec_list[i].tail_name)){
			continue;
		}
		if(g_pt_lv_f_file_browser->exec_list[i].func){
			g_pt_lv_f_file_browser->exec_list[i].func(g_pt_lv_f_file_browser->dir_path,text);
		}
	}
	    
}

static void lv_f_tool_dir_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code != LV_EVENT_CLICKED) {
		return;
	}
	char *text = (char *)lv_list_get_btn_text(g_pt_lv_f_file_browser->list, obj);
	LV_LOG_USER("Clicked: %s", text);
	char path[FILE_PATH_MAX];
	memcpy(path,g_pt_lv_f_file_browser->dir_path,FILE_PATH_MAX);
    if( 0 ==  strcmp(". .",text)){
		int res = file_rover_get_row_up(path,g_pt_lv_f_file_browser->dir_path, &g_pt_lv_f_file_browser->rows);
		if(res == 0){//更新列表
			lv_f_tool_file_update_list(g_pt_lv_f_file_browser->list);
		}
	}else{
		strcat(path,text);
		int res = file_rover_get_row_down(path, &g_pt_lv_f_file_browser->rows);
		if(res == 0){//更新列表
			memcpy(g_pt_lv_f_file_browser->dir_path,path,FILE_PATH_MAX);
			lv_f_tool_file_update_list(g_pt_lv_f_file_browser->list);
		}
	}
}
/*
 *  函数名：   static void lv_f_tool_file_browser_init(void)
 *  输入参数： 无
 *  返回值：   true-初始化成功 false-初始化失败
 *  函数作用： 应用界面初始化
*/
static bool lv_f_tool_file_browser_init(void)
{
	lv_obj_t * btn;
	lv_obj_t * list1 = g_pt_lv_f_file_browser->list;
	f_row_list_t *p_rows = &g_pt_lv_f_file_browser->rows;
	char *path = g_pt_lv_f_file_browser->dir_path;

	//读取当前文件列表
	int res = file_rover_get_row_cwd(path,p_rows);
	if(res != 0){
		return false;
	}
	
	lv_f_tool_file_update_list(list1);
	return true;
}

static void lv_f_tool_file_clear_list(lv_obj_t * list1)
{

	uint32_t cnt = lv_obj_get_child_cnt(list1);
	for (int i = 0; i < cnt; i++){
		lv_obj_t* obj = lv_obj_get_child(list1, 0);
		lv_obj_del(obj);
	}
	
}

static void lv_f_tool_file_update_list(lv_obj_t * list1)
{
	f_row_list_t *p_rows = &g_pt_lv_f_file_browser->rows;
	char *path = g_pt_lv_f_file_browser->dir_path;
	lv_obj_t * btn;
	lv_obj_t * label;
	char buf[100]={0};

	lv_f_tool_file_clear_list(list1);

	lv_list_add_text(list1, path);
	if (0 != strcmp(path, "/"))
    {
		btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, ". .");
		lv_obj_add_event_cb(btn, lv_f_tool_dir_event_handler, LV_EVENT_CLICKED, NULL);
	}

	for(int i = 0; i < p_rows->nfiles;i++){
		if(file_rover_is_dir(p_rows->row[i].mode)){
			btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, p_rows->row[i].name);
    		lv_obj_add_event_cb(btn, lv_f_tool_dir_event_handler, LV_EVENT_CLICKED, NULL);
		}else{
			btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, p_rows->row[i].name);
			lv_obj_add_event_cb(btn, lv_f_tool_file_event_handler, LV_EVENT_CLICKED, NULL);
			label = lv_label_create(btn);
			lv_obj_align(label,LV_ALIGN_RIGHT_MID,-10,0);
			file_rover_get_size_buf(buf,p_rows->row[i].size);
			lv_label_set_text(label,buf);
		}
	}
}