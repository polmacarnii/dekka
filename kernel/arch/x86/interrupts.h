#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <types.h>

/* estado de registros que arma el stub de interrupcion, el layout debe
 * coincidir */
struct regs {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

/* Excepciones de la CPU */
#define ISR_COUNT 32

/* El PIC se remapea a estos vectores, asi que la IRQ n usa el vector 32 + n */
#define IRQ_BASE_VECTOR 32
#define IRQ_COUNT 16

/* todas estas se llaman desde carch(), antes de kmain() */
void gdt_init(void);
void idt_init(void);
void irq_init(void);

/* Handler comun de excepciones (su implementacion esta en isr.c) */
void exception_handler_c(struct regs *r);

/* Handler comun de IRQs */
void irq_handler_c(struct regs *r);

#endif
