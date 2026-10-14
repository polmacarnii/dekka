#ifndef CPU_H
#define CPU_H

#include <types.h>

/* para los puertos de entrada y salida */
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

/* retrasar brevemente el puerto para darle tiempo al driver */
void io_wait(void);

void idt_load(const void *idt_ptr);

#endif
