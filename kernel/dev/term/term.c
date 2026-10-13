#include <dev/fb.h>
#include <dev/term.h>
#include <misc/font8x8.h>
#include <text.h>
#include <types.h>

#define TERM_FW 8
#define TERM_FH 8
#define TERM_TAB 4

static rgb_t term_fg = {255, 255, 255};
static rgb_t term_bg = {0, 0, 0};

static uint32_t cur_x = 0;
static uint32_t cur_y = 0;
static uint32_t term_w = 0;
static uint32_t term_h = 0;

static void draw_cell(char c, uint32_t x, uint32_t y) {
  video_fill_rect(x, y, TERM_FW, TERM_FH, term_bg);
  draw_char(c, x, y, term_fg);
}

void term_init(void) {
  term_w = video_get_width();
  term_h = video_get_height();
  video_fill_screen(term_bg);
}

void term_set_color(rgb_t fg, rgb_t bg) {
  term_fg = fg;
  term_bg = bg;
}

void term_putc(char c) {
  if (term_w == 0 || term_h == 0)
    return;

  uint32_t cols = term_w / TERM_FW;
  uint32_t rows = term_h / TERM_FH;

  switch (c) {
  case '\n':
    cur_x = 0;
    cur_y++;
    return;
  case '\r':
    cur_x = 0;
    return;
  case '\t':
    cur_x = (cur_x + TERM_TAB) & ~(TERM_TAB - 1);
    break;
  default:
    draw_cell(c, cur_x * TERM_FW, cur_y * TERM_FH);
    cur_x++;
    break;
  }

  if (cur_x >= cols) {
    cur_x = 0;
    cur_y++;
  }

  if (cur_y >= rows) {
    video_scroll(TERM_FH, term_bg);
    cur_y--;
  }
}

void term_puts(const char *s) {
  while (*s)
    term_putc(*s++);
}
