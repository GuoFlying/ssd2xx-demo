#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include "lv_lib_freetype/lv_freetype.h"
#include <assert.h>

void f_app_main(int argc, char *argv[])
{
    char *path = NULL;

    if (argc >= 4)
    {
        path = argv[3];
    }
    else
    {
        fprintf(stderr, "error: Usage: %s [w,h] [path]\r\n"
                        "eg: %s 1024 600 ./res/test.ttf ",
                argv[0], argv[0]);
        assert(argv[3]);
        return;
    }

 /*Init freetype library
   *Cache max 64 faces and 1 size*/
  lv_freetype_init(64, 1, 0);

  /*Create a font*/
  static lv_ft_info_t info;
  info.name = path;
  info.weight = 46;
  info.style = FT_FONT_STYLE_NORMAL;
  lv_ft_font_init(&info);

  /*Create style with the new font*/
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, info.font);

  /*Create a label with the new style*/
  lv_obj_t * label = lv_label_create(lv_scr_act());
  lv_obj_add_style(label, &style, 0);
  lv_label_set_text(label, "你好 福莱鹰 ! Hello world");
  lv_obj_center(label);
}