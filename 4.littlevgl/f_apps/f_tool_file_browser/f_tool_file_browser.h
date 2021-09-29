#ifndef _F_TOOL_FILE_BROWSER_H_
#define _F_TOOL_FILE_BROWSER_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl/lvgl.h"

typedef void (*file_exec_func_t)(char *,char *);
typedef struct file_exec_func_list_t_
{
    file_exec_func_t func;
    char *tail_name;
}file_exec_func_list_t;

void lv_f_tool_file_browser(file_exec_func_list_t *exec_list, int exec_count);

#ifdef __cplusplus
}
#endif
#endif