#include "../base_main.fc"
#include "lvgl/examples/lv_examples.h"

typedef void (*func_t)(void);
func_t funs[] = {
    lv_example_animimg_1,
    lv_example_arc_1,
    lv_example_arc_2,
    lv_example_bar_1,
    lv_example_bar_2,
    lv_example_bar_3,
    lv_example_bar_4,
    lv_example_bar_5,
    lv_example_bar_6,
    lv_example_btn_1,//9

    lv_example_btn_2,
    lv_example_btn_3,
    lv_example_btnmatrix_1,
    lv_example_btnmatrix_2,
    lv_example_btnmatrix_3,
    lv_example_calendar_1,
    lv_example_canvas_1,
    lv_example_canvas_2,
    lv_example_chart_1,
    lv_example_chart_2,//19

    lv_example_chart_3,
    lv_example_chart_4,
    lv_example_chart_5,
    lv_example_chart_6,
    lv_example_chart_7,
    lv_example_checkbox_1,
    lv_example_colorwheel_1,
    lv_example_dropdown_1,
    lv_example_dropdown_2,
    lv_example_dropdown_3,//29

    lv_example_img_1,
    lv_example_img_2,
    lv_example_img_3,
    lv_example_img_4,
    lv_example_imgbtn_1,
    lv_example_keyboard_1,
    lv_example_btnmatrix_1,
    lv_example_label_1,
    lv_example_label_2,
    lv_example_label_2,
    //lv_example_label_3,//39

    lv_example_led_1,
    lv_example_line_1,
    lv_example_list_1,
    lv_example_meter_1,
    lv_example_meter_2,
    lv_example_meter_3,
    lv_example_meter_4,
    lv_example_msgbox_1,
    lv_example_obj_1,
    lv_example_obj_2,//49

    lv_example_roller_1,
    lv_example_roller_2,
    lv_example_roller_3,
    lv_example_slider_1,
    lv_example_slider_2,
    lv_example_slider_3,
    lv_example_span_1,
    lv_example_spinbox_1,
    lv_example_switch_1,
    lv_example_table_1,//59

    lv_example_table_2,
    lv_example_tabview_1,
    lv_example_tabview_2,
    lv_example_textarea_1,
    lv_example_textarea_2,
    lv_example_textarea_3,
    lv_example_tileview_1,
    lv_example_win_1,
    lv_example_switch_1,
    lv_example_switch_1,//69

};

void f_app_main(int argc, char *argv[])
{
    int demo_id = 10;
    if (argc >= 4)
    {
        demo_id = atoi(argv[3]);
    }
    else
    {
        fprintf(stderr,"Warring: Usage: %s [w,h] [deme_id]\r\n"
               "deme_id: 0 1 2 3 \r\n"
               "eg: %s 1024 600 3 ",
               argv[0], argv[0]);
    }

    /*Create a Demo*/
    int sum = sizeof(funs)/sizeof(func_t);
    funs[demo_id%sum]();
}