#ifndef COLORS_H
#define COLORS_H

/* Cada arquitectura puede tener sus colores propios */
#include <arch.h>

#define COLOR_BLACK ((rgb_t){0x00, 0x00, 0x00})
#define COLOR_WHITE ((rgb_t){0xFF, 0xFF, 0xFF})

#define COLOR_RED ((rgb_t){0xFF, 0x00, 0x00})
#define COLOR_GREEN ((rgb_t){0x00, 0xFF, 0x00})
#define COLOR_BLUE ((rgb_t){0x00, 0x00, 0xFF})

#define COLOR_YELLOW ((rgb_t){0xFF, 0xFF, 0x00})

#endif
