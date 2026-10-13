#include <arch.h>
#include <klib/mem.h>
#include <types.h>

static uint32_t rgb_to_pixel(rgb_t c) {
  return ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | (uint32_t)c.b;
}

static uint32_t *fb = NULL;

uint32_t g_screen_width;
uint32_t g_screen_height;

static uint32_t screen_width;
static uint32_t screen_height;
static uint32_t screen_pitch;

/* conectar las variables nuestras con las del fb */
void video_init(uint32_t *framebuffer, uint32_t width, uint32_t height,
                uint32_t pitch) {
  fb = framebuffer;

  screen_width = width;
  screen_height = height;
  screen_pitch = pitch;

  g_screen_width = width;
  g_screen_height = height;
}

void video_put_pixel(int x, int y, rgb_t color) {
  /* Checar que el pixel este dentro de la pantalla */
  if (x < 0 || y < 0 || x >= (int)screen_width || y >= (int)screen_height) {
    return;
  }

  fb[y * (screen_pitch / sizeof(uint32_t)) + x] = rgb_to_pixel(color);
}

void video_fill_screen(rgb_t color) {
  uint32_t pixels_per_row = screen_pitch / sizeof(uint32_t);
  uint32_t pixel = rgb_to_pixel(color);

  for (uint32_t y = 0; y < screen_height; y++) {
    uint32_t *row = &fb[y * pixels_per_row];

    for (uint32_t x = 0; x < screen_width; x++) {
      row[x] = pixel;
    }
  }
}

void video_fill_rect(int x, int y, int w, int h, rgb_t color) {
  if (w <= 0 || h <= 0)
    return;

  if (x < 0) {
    w += x;
    x = 0;
  }

  if (y < 0) {
    h += y;
    y = 0;
  }

  if (x + w > (int)screen_width)
    w = screen_width - x;

  if (y + h > (int)screen_height)
    h = screen_height - y;

  if (w <= 0 || h <= 0)
    return;

  uint32_t pixels_per_row = screen_pitch / sizeof(uint32_t);
  uint32_t pixel = rgb_to_pixel(color);

  for (int iy = 0; iy < h; iy++) {
    uint32_t *row = &fb[(y + iy) * pixels_per_row + x];

    for (int ix = 0; ix < w; ix++) {
      row[ix] = pixel;
    }
  }
}

uint32_t video_get_width(void) { return screen_width; }

uint32_t video_get_height(void) { return screen_height; }

uint32_t *video_get_fb(void) { return fb; }

uint32_t video_get_pitch(void) { return screen_pitch; }

void video_scroll(int lines, rgb_t bg_color) {
  if (lines <= 0)
    return;

  if (lines >= (int)screen_height) {
    video_fill_screen(bg_color);
    return;
  }

  uint32_t pixels_per_row = screen_pitch / sizeof(uint32_t);

  uint32_t rows_to_move = screen_height - lines;

  memmove(fb, fb + lines * pixels_per_row, rows_to_move * screen_pitch);

  uint32_t pixel = rgb_to_pixel(bg_color);

  for (uint32_t y = rows_to_move; y < screen_height; y++) {
    uint32_t *row = &fb[y * pixels_per_row];

    for (uint32_t x = 0; x < screen_width; x++) {
      row[x] = pixel;
    }
  }
}
