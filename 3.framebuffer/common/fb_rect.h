#ifndef _FB_RECT_H_
#define _FB_RECT_H_
int fb_rect_init(int fd);
void fb_rect_draw(int x0, int y0, int width, int height, int color);
void fb_rect_draw_full(int color);
#endif