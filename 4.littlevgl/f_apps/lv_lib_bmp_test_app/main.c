#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include "lv_lib_bmp/lv_bmp.h"
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
        fprintf(stderr, "error: Usage: %s [w,h] [bmp_path]\r\n"
                        "eg: %s 1024 600 ./res/test.bmp ",
                argv[0], argv[0]);
        assert(argv[3]);
        return;
    }
    lv_fs_if_init();

    lv_bmp_init();

    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, path);
    lv_obj_center(img);

}