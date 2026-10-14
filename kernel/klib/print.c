#include <dev/term.h>
#include <print.h>
#include <types.h>

/* Seria un poco tonto meter stdout y esas cosas aqui en el kernel, asi que esto
 * es el remplazo */
void printc(char c) { term_putc(c); }

void print(const char *s) { term_puts(s); }

static void print_uns(uint32_t n, uint32_t base, char upper) {
  const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
  char buf[33];
  uint32_t i = 0;

  if (n == 0)
    buf[i++] = '0';

  while (n != 0) {
    buf[i++] = digits[n % base];
    n /= base;
  }

  while (i > 0)
    term_putc(buf[--i]);
}

void print_dec(int32_t n) {
  if (n < 0) {
    term_putc('-');
    print_uns((uint32_t)n, 10, 0);
    return;
  }

  print_uns((uint32_t)n, 10, 0);
}

void print_uint(uint32_t n) { print_uns(n, 10, 0); }

void print_hex(uint32_t n) {
  term_putc('0');
  term_putc('x');
  print_uns(n, 16, 0);
}

void print_ptr(const void *p) { print_hex((uint32_t)(uintptr_t)p); }

void vprint_fmt(const char *fmt, va_list ap) {
  while (*fmt) {
    if (*fmt != '%') {
      term_putc(*fmt++);
      continue;
    }

    fmt++;

    switch (*fmt) {
    case 's':
      term_puts(va_arg(ap, const char *));
      break;
    case 'c':
      term_putc((char)va_arg(ap, int));
      break;
    case 'd':
    case 'i':
      print_dec(va_arg(ap, int32_t));
      break;
    case 'u':
      print_uint(va_arg(ap, uint32_t));
      break;
    case 'x':
      print_hex(va_arg(ap, uint32_t));
      break;
    case 'X':
      term_putc('0');
      term_putc('x');
      print_uns(va_arg(ap, uint32_t), 16, 1);
      break;
    case 'p':
      print_ptr(va_arg(ap, void *));
      break;
    case '%':
      term_putc('%');
      break;
    case '\0':
      return;
    default:
      /* Especificador desconocido: se devuelve tal cual */
      term_putc('%');
      term_putc(*fmt);
      break;
    }

    if (*fmt)
      fmt++;
  }
}

void print_fmt(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprint_fmt(fmt, ap);
  va_end(ap);
}

void print_color(rgb_t fg, const char *s) {
  rgb_t old_fg, old_bg;

  term_get_color(&old_fg, &old_bg);
  term_set_color(fg, old_bg);
  term_puts(s);
  term_set_color(old_fg, old_bg);
}

void print_colorf(rgb_t fg, const char *fmt, ...) {
  va_list ap;
  rgb_t old_fg, old_bg;

  term_get_color(&old_fg, &old_bg);
  term_set_color(fg, old_bg);

  va_start(ap, fmt);
  vprint_fmt(fmt, ap);
  va_end(ap);

  term_set_color(old_fg, old_bg);
}
