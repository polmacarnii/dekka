#include <arch.h>
#include <cpu.h>

uint8_t inb(uint16_t port) {
    uint8_t val;

    __asm__ volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void io_wait(void) { outb(0x80, 0); }

void interrupts_enable(void) { __asm__ volatile("sti"); }

void interrupts_disable(void) { __asm__ volatile("cli"); }
