#ifndef FB_H
#define FB_H

#include <arch.h>

void video_init(uint32_t *framebuffer, uint32_t w, uint32_t h, uint32_t pitch);

void video_put_pixel(int x, int y, rgb_t color);

void video_fill_rect(int x, int y, int w, int h, rgb_t color);

uint32_t video_get_width();
uint32_t video_get_height();
uint32_t *video_get_fb(void);
uint32_t video_get_pitch(void);

void video_scroll(int lines, rgb_t bg_color);

void video_fill_screen(rgb_t color);

#endif
