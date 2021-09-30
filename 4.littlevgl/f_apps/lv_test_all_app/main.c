#include "../base_main.fc"
#include "../../lvgl/examples/lv_examples.h"

#define DEF_EXAMPLE_FUNC(a) a, #a

typedef void (*func_t)(void);
struct _examples_func_t
{
    func_t func;
    char *fun_name;
} examples_func[] = {
    DEF_EXAMPLE_FUNC(lv_example_animimg_1),
    DEF_EXAMPLE_FUNC(lv_example_arc_1),
    DEF_EXAMPLE_FUNC(lv_example_arc_2),
    DEF_EXAMPLE_FUNC(lv_example_bar_1),
    DEF_EXAMPLE_FUNC(lv_example_bar_2),
    DEF_EXAMPLE_FUNC(lv_example_bar_3),
    DEF_EXAMPLE_FUNC(lv_example_bar_4),
    DEF_EXAMPLE_FUNC(lv_example_bar_5),
    DEF_EXAMPLE_FUNC(lv_example_bar_6),
    DEF_EXAMPLE_FUNC(lv_example_btn_1), //9

    DEF_EXAMPLE_FUNC(lv_example_btn_2),
    DEF_EXAMPLE_FUNC(lv_example_btn_3),
    DEF_EXAMPLE_FUNC(lv_example_btnmatrix_1),
    DEF_EXAMPLE_FUNC(lv_example_btnmatrix_2),
    DEF_EXAMPLE_FUNC(lv_example_btnmatrix_3),
    DEF_EXAMPLE_FUNC(lv_example_calendar_1),
    DEF_EXAMPLE_FUNC(lv_example_canvas_1),
    DEF_EXAMPLE_FUNC(lv_example_canvas_2),
    DEF_EXAMPLE_FUNC(lv_example_chart_1),
    DEF_EXAMPLE_FUNC(lv_example_chart_2), //19

    DEF_EXAMPLE_FUNC(lv_example_chart_3),
    DEF_EXAMPLE_FUNC(lv_example_chart_4),
    DEF_EXAMPLE_FUNC(lv_example_chart_5),
    DEF_EXAMPLE_FUNC(lv_example_chart_6),
    DEF_EXAMPLE_FUNC(lv_example_chart_7),
    DEF_EXAMPLE_FUNC(lv_example_checkbox_1),
    DEF_EXAMPLE_FUNC(lv_example_colorwheel_1),
    DEF_EXAMPLE_FUNC(lv_example_dropdown_1),
    DEF_EXAMPLE_FUNC(lv_example_dropdown_2),
    DEF_EXAMPLE_FUNC(lv_example_dropdown_3), //29

    DEF_EXAMPLE_FUNC(lv_example_img_1),
    DEF_EXAMPLE_FUNC(lv_example_img_2),
    DEF_EXAMPLE_FUNC(lv_example_img_3),
    DEF_EXAMPLE_FUNC(lv_example_img_4),
    DEF_EXAMPLE_FUNC(lv_example_imgbtn_1),
    DEF_EXAMPLE_FUNC(lv_example_keyboard_1),
    DEF_EXAMPLE_FUNC(lv_example_btnmatrix_1),
    DEF_EXAMPLE_FUNC(lv_example_label_1),
    DEF_EXAMPLE_FUNC(lv_example_label_2),
    DEF_EXAMPLE_FUNC(lv_example_led_1),//39

    DEF_EXAMPLE_FUNC(lv_example_line_1),
    DEF_EXAMPLE_FUNC(lv_example_list_1),
    DEF_EXAMPLE_FUNC(lv_example_list_2),
    DEF_EXAMPLE_FUNC(lv_example_meter_1),
    DEF_EXAMPLE_FUNC(lv_example_meter_2),
    DEF_EXAMPLE_FUNC(lv_example_meter_3),
    DEF_EXAMPLE_FUNC(lv_example_meter_4),
    DEF_EXAMPLE_FUNC(lv_example_msgbox_1),
    DEF_EXAMPLE_FUNC(lv_example_obj_1),
    DEF_EXAMPLE_FUNC(lv_example_obj_2), //49

    DEF_EXAMPLE_FUNC(lv_example_roller_1),
    DEF_EXAMPLE_FUNC(lv_example_roller_2),
    DEF_EXAMPLE_FUNC(lv_example_roller_3),
    DEF_EXAMPLE_FUNC(lv_example_slider_1),
    DEF_EXAMPLE_FUNC(lv_example_slider_2),
    DEF_EXAMPLE_FUNC(lv_example_slider_3),
    DEF_EXAMPLE_FUNC(lv_example_span_1),
    DEF_EXAMPLE_FUNC(lv_example_spinbox_1),
    DEF_EXAMPLE_FUNC(lv_example_switch_1),
    DEF_EXAMPLE_FUNC(lv_example_table_1), //59

    DEF_EXAMPLE_FUNC(lv_example_table_2),
    DEF_EXAMPLE_FUNC(lv_example_tabview_1),
    DEF_EXAMPLE_FUNC(lv_example_tabview_2),
    DEF_EXAMPLE_FUNC(lv_example_textarea_1),
    DEF_EXAMPLE_FUNC(lv_example_textarea_2),
    DEF_EXAMPLE_FUNC(lv_example_textarea_3),
    DEF_EXAMPLE_FUNC(lv_example_tileview_1),
    DEF_EXAMPLE_FUNC(lv_example_win_1),
    DEF_EXAMPLE_FUNC(lv_example_switch_1),
    DEF_EXAMPLE_FUNC(lv_example_switch_1), //69

    DEF_EXAMPLE_FUNC(lv_example_style_1),
    DEF_EXAMPLE_FUNC(lv_example_style_2),
    DEF_EXAMPLE_FUNC(lv_example_style_3),
    DEF_EXAMPLE_FUNC(lv_example_style_4),
    DEF_EXAMPLE_FUNC(lv_example_style_5),
    DEF_EXAMPLE_FUNC(lv_example_style_6),
    DEF_EXAMPLE_FUNC(lv_example_style_7),
    DEF_EXAMPLE_FUNC(lv_example_style_8),
    DEF_EXAMPLE_FUNC(lv_example_style_9),
    DEF_EXAMPLE_FUNC(lv_example_style_10), //79

    DEF_EXAMPLE_FUNC(lv_example_style_11),
    DEF_EXAMPLE_FUNC(lv_example_style_12),
    DEF_EXAMPLE_FUNC(lv_example_style_13),
    DEF_EXAMPLE_FUNC(lv_example_style_14),
    DEF_EXAMPLE_FUNC(lv_example_scroll_1),
    DEF_EXAMPLE_FUNC(lv_example_scroll_2),
    DEF_EXAMPLE_FUNC(lv_example_scroll_3),
    DEF_EXAMPLE_FUNC(lv_example_scroll_4),
    DEF_EXAMPLE_FUNC(lv_example_scroll_5),
    DEF_EXAMPLE_FUNC(lv_example_scroll_6), //89

    DEF_EXAMPLE_FUNC(lv_example_event_1),
    DEF_EXAMPLE_FUNC(lv_example_event_2),
    DEF_EXAMPLE_FUNC(lv_example_event_3),
    DEF_EXAMPLE_FUNC(lv_example_anim_1),
    DEF_EXAMPLE_FUNC(lv_example_anim_2),
    DEF_EXAMPLE_FUNC(lv_example_anim_3),
    DEF_EXAMPLE_FUNC(lv_example_anim_timeline_1),
    DEF_EXAMPLE_FUNC(lv_example_flex_1),
    DEF_EXAMPLE_FUNC(lv_example_flex_2),
    DEF_EXAMPLE_FUNC(lv_example_flex_3), //99

    DEF_EXAMPLE_FUNC(lv_example_flex_4),
    DEF_EXAMPLE_FUNC(lv_example_flex_5),
    DEF_EXAMPLE_FUNC(lv_example_flex_6),
    DEF_EXAMPLE_FUNC(lv_example_grid_1),
    DEF_EXAMPLE_FUNC(lv_example_grid_2),
    DEF_EXAMPLE_FUNC(lv_example_grid_3),
    DEF_EXAMPLE_FUNC(lv_example_grid_4),
    DEF_EXAMPLE_FUNC(lv_example_grid_5),
    DEF_EXAMPLE_FUNC(lv_example_grid_6),
    DEF_EXAMPLE_FUNC(lv_example_snapshot_1), //109
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
        for (int i = 0; i < sizeof(examples_func) / sizeof(struct _examples_func_t); i++)
        {
            printf("DEMO ( id,name ):( %d, %s )\r\n", i, examples_func[i].fun_name);
        }

        fprintf(stderr, "Warring: Usage: %s [w,h] [deme_id]\r\n"
                        "eg: %s 1024 600 10 \r\n",
                argv[0], argv[0]);
    }

    /*Create a Demo*/
    int sum = sizeof(examples_func) / sizeof(struct _examples_func_t);
    examples_func[demo_id % sum].func();
    printf("DEMO ( id,name ):( %d, %s )\r\n", demo_id % sum, examples_func[demo_id % sum].fun_name);
}