#ifndef ARCH_H
#define ARCH_H

#include <types.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

typedef struct {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;

} framebuffer_t;

void halt();

typedef void (*irq_handler_t)(void);

/* Registra el handler de una linea IRQ (0..15) */
int irq_register(uint8_t irq, irq_handler_t handler);

int irq_enable(uint8_t irq);

void interrupts_enable(void);
void interrupts_disable(void);

/* Multitarea */
void swtch(uchar **old, uchar *new);

#endif
