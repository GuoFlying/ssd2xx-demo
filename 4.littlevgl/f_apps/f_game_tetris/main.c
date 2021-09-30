#include "../base_main.fc"
#include "lvgl/lvgl.h"
#include <assert.h>
#include "f_game_tetris.h"

void f_app_main(int argc, char *argv[])
{
    f_game_tetris();
}