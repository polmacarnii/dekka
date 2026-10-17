#include <interrupts.h>
#include <types.h>

#define GDT_ENTRIES 3

#define GDT_NULL 0
#define GDT_KERNEL_CS 1 /* 0x08 */
#define GDT_KERNEL_DS 2 /* 0x10 */

struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gdt_ptr;

static void gdt_set_gate(uint8_t num, uint32_t base, uint32_t limit,
                         uint8_t access, uint8_t granularity) {
  gdt[num].base_low = base & 0xFFFF;
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;
  gdt[num].limit_low = limit & 0xFFFF;
  gdt[num].granularity = (limit >> 16) & 0x0F;
  gdt[num].granularity |= granularity;
  gdt[num].access = access;
}

static void gdt_load(void) {
  __asm__ volatile("lgdt %0" : : "m"(gdt_ptr));

  /* Toca recargar los selectores de segmento porque los del bootloader ya no
   * existen en la nueva tabla */
  __asm__ volatile("movw $0x10, %%ax\n\t"
                   "movw %%ax, %%ds\n\t"
                   "movw %%ax, %%es\n\t"
                   "movw %%ax, %%fs\n\t"
                   "movw %%ax, %%gs\n\t"
                   "movw %%ax, %%ss\n\t"
                   "ljmp $0x08, $1f\n\t"
                   "1:"
                   :
                   :
                   : "eax", "memory");
}

void gdt_init(void) {
  gdt_ptr.limit = sizeof(gdt) - 1;
  gdt_ptr.base = (uint32_t)(uintptr_t)&gdt;

  /* el descriptor 0 debe estar en 0, si no el CPU lo toma como selector valido
   * y usa ese descriptor al referenciar el 0 */
  gdt[GDT_NULL].base_low = 0;
  gdt[GDT_NULL].base_middle = 0;
  gdt[GDT_NULL].base_high = 0;
  gdt[GDT_NULL].limit_low = 0;
  gdt[GDT_NULL].granularity = 0;
  gdt[GDT_NULL].access = 0;

  /* Codigo de kernel en ring 0*/
  /* 4KiB de granularidad */
  gdt_set_gate(GDT_KERNEL_CS, 0, 0xFFFFF, 0x9A, 0xCF);

  /* Datos de kernel */
  gdt_set_gate(GDT_KERNEL_DS, 0, 0xFFFFF, 0x92, 0xCF);

  gdt_load();
}
