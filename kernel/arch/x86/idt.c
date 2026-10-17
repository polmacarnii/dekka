/* Tabla de descriptores de interrupcion (IDT). */

#include <cpu.h>
#include <interrupts.h>
#include <klib/mem.h>
#include <types.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Los stubs de excepciones (isr.asm) y los de IRQs (irq.asm) */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0_handler(void);
extern void irq1_handler(void);
extern void irq2_handler(void);
extern void irq3_handler(void);
extern void irq4_handler(void);
extern void irq5_handler(void);
extern void irq6_handler(void);
extern void irq7_handler(void);
extern void irq8_handler(void);
extern void irq9_handler(void);
extern void irq10_handler(void);
extern void irq11_handler(void);
extern void irq12_handler(void);
extern void irq13_handler(void);
extern void irq14_handler(void);
extern void irq15_handler(void);

static void (*isr_table[ISR_COUNT])(void) = {
    isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,  isr8,  isr9,  isr10,
    isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19, isr20, isr21,
    isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};

static void (*irq_table[IRQ_COUNT])(void) = {
    irq0_handler,  irq1_handler,  irq2_handler,  irq3_handler,
    irq4_handler,  irq5_handler,  irq6_handler,  irq7_handler,
    irq8_handler,  irq9_handler,  irq10_handler, irq11_handler,
    irq12_handler, irq13_handler, irq14_handler, irq15_handler};

static struct idt_entry idt[256];
static struct idt_ptr idtp;

#define GDT_KERNEL_CS 0x08
#define IDT_FLAG_KERNEL 0x8E /* presente, DPL 0 y un gate de 32 bits */

/* Cuales excepciones empuujan codigo de error esta definido en el fichero
 * isr.asm */
/* las que empujan unicamente el vector dejan un  en suu lugar como ya se
 * menciono asi el layout es coherente */
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector,
                         uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void idt_init(void) {
    memset(idt, 0, sizeof(idt));

    for (int i = 0; i < ISR_COUNT; i++)
        idt_set_gate(i, (uint32_t)(uintptr_t)isr_table[i], GDT_KERNEL_CS,
                     IDT_FLAG_KERNEL);

    /* El PIC se remapea en irq_init(), por eso las IRQs arrancan en 32 */
    for (int i = 0; i < IRQ_COUNT; i++)
        idt_set_gate(IRQ_BASE_VECTOR + i, (uint32_t)(uintptr_t)irq_table[i],
                     GDT_KERNEL_CS, IDT_FLAG_KERNEL);

    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)(uintptr_t)&idt;

    idt_load(&idtp);
}
