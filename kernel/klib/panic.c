#include <arch.h>
#include <dev/fb.h>
#include <dev/term.h>
#include <klib/panic.h>
#include <text.h>

void panic(const char *msg) {
    rgb_t red = {255, 0, 0};
    rgb_t white = {255, 255, 255};

    video_fill_screen(red);
    term_set_color(white, red);
    term_putc('\n');
    term_puts("KERNEL PANIC: ");
    term_puts(msg);
    term_putc('\n');

    for (;;)
        halt();
}
