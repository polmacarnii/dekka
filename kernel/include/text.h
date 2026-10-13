#ifndef TEXT_H
#define TEXT_H

#include <arch.h>

void draw_char(char c, int x, int y, rgb_t color);
void draw_string(const char *str, int x, int y, rgb_t color);

#endif
