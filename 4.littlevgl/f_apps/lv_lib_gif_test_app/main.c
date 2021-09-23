#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include "lv_lib_gif/lv_gif.h"
#include <assert.h>
#include "lv_fs_if/lv_fs_if.h"

void f_app_main(int argc, char *argv[])
{
    char path[256] = {0};

    if (argc >= 4)
    {
        sprintf(path,"S%s",argv[3]);
    }
    else
    {
        fprintf(stderr, "error: Usage: %s [w,h] [path]\r\n"
                        "eg: %s 1024 600 ./res/example.gif ",
                argv[0], argv[0]);
        assert(argv[3]);
        return;
    }

    lv_fs_if_init();
    lv_obj_t * img = lv_gif_create_from_file(lv_scr_act(), path);
    lv_obj_center(img);

#if 0
    extern const uint8_t example_gif_map[];  /*"lv_lib_gif/example_gif.h"*/
    lv_obj_t * img = lv_gif_create_from_data(lv_scr_act(), example_gif_map);
    lv_obj_center(img);

    lv_anim_t a;
    lv_anim_init(&a);

    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_var(&a, img);
    lv_anim_set_time(&a, 10000);
    lv_anim_set_values(&a, 512, LV_IMG_ZOOM_NONE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_ready_cb(&a, NULL);

    lv_anim_start(&a);
#endif
}