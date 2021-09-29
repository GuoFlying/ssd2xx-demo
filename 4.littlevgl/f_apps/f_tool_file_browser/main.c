#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include <assert.h>
#include "f_tool_file_browser.h"
#include "lv_lib_png/lv_png.h"
#include "lv_lib_split_jpg/lv_sjpg.h"
#include "lv_fs_if/lv_fs_if.h"
#include "lv_lib_gif/lv_gif.h"
#include "lv_lib_bmp/lv_bmp.h"
#include "lv_lib_freetype/lv_freetype.h"

static void show_png(char *path, char *file_name)
{
    char buff[1024] = {0};
    lv_obj_t *mbox1 = lv_msgbox_create(NULL, file_name, path, NULL, true);
    lv_obj_set_size(mbox1,LV_HOR_RES-20,LV_VER_RES-20);
    lv_obj_center(mbox1);

    sprintf(buff, "%s%s", path, file_name);

    lv_obj_t *img = lv_img_create(mbox1);
    lv_img_set_src(img, buff);
}

static void show_jpg(char *path, char *file_name)
{
    char buff[2048] = {0};
    char cwd[1024] =  {0};
    lv_obj_t *mbox1 = lv_msgbox_create(NULL, file_name, path, NULL, true);
    lv_obj_set_size(mbox1,LV_HOR_RES-20,LV_VER_RES-20);
    lv_obj_center(mbox1);
    getcwd(cwd, 1024);
    char *dirname = cwd;
    strcat(buff,"S");
    while(dirname = strchr(dirname,'/')){
        dirname += 1;
        strcat(buff,"../");
    }
    buff[strlen(buff)-1] = '\0';
    strcat(buff,path);
    strcat(buff,file_name);
    lv_obj_t *img = lv_img_create(mbox1);
    lv_img_set_src(img, buff);
}

static void show_gif(char *path, char *file_name)
{
    char buff[2048] = {0};
    char cwd[1024] =  {0};
    lv_obj_t *mbox1 = lv_msgbox_create(NULL, file_name, path, NULL, true);
    lv_obj_set_size(mbox1,LV_HOR_RES-20,LV_VER_RES-20);
    lv_obj_center(mbox1);
    getcwd(cwd, 1024);
    char *dirname = cwd;
    strcat(buff,"S");
    while(dirname = strchr(dirname,'/')){
        dirname += 1;
        strcat(buff,"../");
    }
    buff[strlen(buff)-1] = '\0';
    strcat(buff,path);
    strcat(buff,file_name);
    // printf("%s\r\n",buff);
    lv_obj_t * img = lv_gif_create_from_file(mbox1, buff);
}

static void show_bmp(char *path, char *file_name)
{
    char buff[2048] = {0};
    char cwd[1024] =  {0};
    lv_obj_t *mbox1 = lv_msgbox_create(NULL, file_name, path, NULL, true);
    lv_obj_set_size(mbox1,LV_HOR_RES-20,LV_VER_RES-20);
    lv_obj_center(mbox1);
    getcwd(cwd, 1024);
    char *dirname = cwd;
    strcat(buff,"S");
    while(dirname = strchr(dirname,'/')){
        dirname += 1;
        strcat(buff,"../");
    }
    buff[strlen(buff)-1] = '\0';
    strcat(buff,path);
    strcat(buff,file_name);
    // printf("%s\r\n",buff);
    lv_obj_t * img = lv_img_create(mbox1);
    lv_img_set_src(img, buff);
}

static void show_ttf(char *path, char *file_name)
{
    char buff[1024] = {0};
    lv_obj_t *mbox1 = lv_msgbox_create(NULL, file_name, path, NULL, true);
    lv_obj_set_size(mbox1,LV_HOR_RES-20,LV_VER_RES-20);
    lv_obj_center(mbox1);

    sprintf(buff, "%s%s", path, file_name);
        /*Create a font*/
    static lv_ft_info_t info;
    info.name = buff;
    info.weight = 32;
    info.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&info);

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(mbox1);
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "English:Hello mFlying!" 
    "\n\nChinese: 你好 福莱鹰 !"
    "\n\nJapanese: ハロー·ワールド。"
    "\n\nKorean: 네오위세이 "
    "\n\nRussian: Привет, мир."
    "\n\nPortuguese: Olá mundo"
    );
}

file_exec_func_list_t open_func[] = {
    {show_png, "png"},
    {show_jpg, "jpg"},
    {show_gif, "gif"},
    {show_bmp, "bmp"},
    {show_ttf, "ttf"},
    {show_ttf, "TTF"},

};
void f_app_main(int argc, char *argv[])
{
    lv_fs_if_init();
    lv_split_jpeg_init();
    lv_png_init();
    lv_bmp_init();
    lv_freetype_init(64, 1, 0);

    lv_f_tool_file_browser(open_func, sizeof(open_func) / sizeof(open_func[0]));
}