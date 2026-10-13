#ifndef TERM_H
#define TERM_H

#include <arch.h>

void term_init(void);
void term_putc(char c);
void term_puts(const char *s);
void term_set_color(rgb_t fg, rgb_t bg);

#endif
