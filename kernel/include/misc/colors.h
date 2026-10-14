#ifndef COLORS_H
#define COLORS_H

/* Los colores son rgb_t */
#include <arch.h>

#define COLOR_BLACK ((rgb_t){0x00, 0x00, 0x00})
#define COLOR_WHITE ((rgb_t){0xFF, 0xFF, 0xFF})

#define COLOR_RED ((rgb_t){0xFF, 0x00, 0x00})
#define COLOR_GREEN ((rgb_t){0x00, 0xFF, 0x00})
#define COLOR_BLUE ((rgb_t){0x00, 0x00, 0xFF})
#define COLOR_YELLOW ((rgb_t){0xFF, 0xFF, 0x00})
#define COLOR_MAGENTA ((rgb_t){0xFF, 0x00, 0xFF})
#define COLOR_CYAN ((rgb_t){0x00, 0xFF, 0xFF})
#define COLOR_GRAY ((rgb_t){0xAA, 0xAA, 0xAA})
#define COLOR_DARKGRAY ((rgb_t){0x55, 0x55, 0x55})

/* Los "bright" de los anteriores: mismo color con los canales al maximo */
#define COLOR_BRIGHT_BLACK ((rgb_t){0x55, 0x55, 0x55})
#define COLOR_BRIGHT_WHITE ((rgb_t){0xFF, 0xFF, 0xFF})
#define COLOR_BRIGHT_RED ((rgb_t){0xFF, 0x55, 0x55})
#define COLOR_BRIGHT_GREEN ((rgb_t){0x55, 0xFF, 0x55})
#define COLOR_BRIGHT_BLUE ((rgb_t){0x55, 0x55, 0xFF})
#define COLOR_BRIGHT_YELLOW ((rgb_t){0xFF, 0xFF, 0x55})
#define COLOR_BRIGHT_MAGENTA ((rgb_t){0xFF, 0x55, 0xFF})
#define COLOR_BRIGHT_CYAN ((rgb_t){0x55, 0xFF, 0xFF})

#endif
