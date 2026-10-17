#include <dev/fb.h>
#include <misc/font8x8.h>
#include <text.h>
#include <types.h>

void draw_char(char c, int x, int y, rgb_t color) {
    for (int row = 0; row < 8; row++) {
        uint8_t bits = font8x8_basic[(uint8_t)c][row];

        for (int col = 0; col < 8; col++) {
            if (bits & (1 << col)) { /* si el bit esta encendido */
                video_put_pixel(x + col, y + row, color);
            }
        }
    }
}

void draw_string(const char *str, int x, int y, rgb_t color) {
    while (*str) {
        draw_char(*str, x, y, color);
        x += 8; /* CAMBIAR 8 SI LA FUENTE CAMBIA DE TAMAÑO! */
        str++;
    }
}
