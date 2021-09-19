#include "../base_main.fc"
#include "lv_demos/lv_demo.h"

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
    switch (demo_id)
    {
    case 0:
        lv_demo_music();
        break;
    case 1:
        lv_demo_stress();
        break;
    case 2:
        lv_demo_keypad_encoder();
        break;
    default:
        lv_demo_widgets();
        break;
    }
}