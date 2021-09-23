#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include "lv_lib_split_jpg/lv_sjpg.h"
#include "lv_fs_if/lv_fs_if.h"
#include <assert.h>

static void timer_cb(struct _lv_timer_t *timer)
{
    static int a = 0;
    lv_obj_t * panel = timer->user_data;
    a++;
    lv_obj_scroll_to_view(lv_obj_get_child(panel, a%3), LV_ANIM_OFF);
    if(a>100){
        lv_timer_del(timer);
    }
}

void f_app_main(int argc, char *argv[])
{
    char *png_path = NULL;

    lv_obj_t * img;

    lv_fs_if_init();

    lv_split_jpeg_init();

    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, 500, 520);
    lv_obj_set_scroll_snap_x(panel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_scroll_dir(panel,LV_DIR_HOR);
    lv_obj_add_flag(panel,LV_OBJ_FLAG_SCROLL_CHAIN);//可滑动
    lv_obj_add_flag(panel, LV_OBJ_FLAG_SCROLL_ONE);
    
    //增加LV_IMG_CACHE_DEF_SIZE可提高缓存
    img = lv_img_create(panel);
    lv_img_set_src(img,  "S./res/1.jpg");
    lv_obj_set_size(img, 480 , 480);

    img = lv_img_create(panel);
    lv_img_set_src(img,  "S./res/2.jpg");
    lv_obj_set_size(img, 480 , 480);
    img = lv_img_create(panel);

    lv_img_set_src(img,  "S./res/3.jpg");
    lv_obj_set_size(img, 480 , 480);
    lv_obj_update_snap(panel, LV_ANIM_ON);

    lv_timer_create(timer_cb,3000,panel);
}
