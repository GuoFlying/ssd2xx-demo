#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include "lv_lib_png/lv_png.h"
#include <assert.h>
void f_app_main(int argc, char *argv[])
{
    char *png_path = NULL;

    if (argc >= 4)
    {
        png_path = argv[3];
    }
    else
    {
        fprintf(stderr, "error: Usage: %s [w,h] [png_path]\r\n"
                        "eg: %s 1024 600 ./res/test.png ",
                argv[0], argv[0]);
        assert(argv[3]);
        return;
    }

    lv_png_init();

    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, png_path);
    lv_obj_center(img);

    lv_anim_t a;
    lv_anim_init(&a);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_var(&a, img);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_values(&a, 512, LV_IMG_ZOOM_NONE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_ready_cb(&a, NULL);

    lv_anim_start(&a);
}