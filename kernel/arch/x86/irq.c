/* PIC (Programmable Interrupt Controller) y dispatch de IRQs */
#include <arch.h>
#include <cpu.h>
#include <interrupts.h>

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20
#define PIC_INIT 0x11
#define PIC_MASTER_MASK 0x04
#define PIC_RAW_MODE 0x01

/* IRQ >= 8: el PIC usa dos bytes para el numero de IRQ */
#define PIC_RAW_BASE 8

/* La linea del maestro que lleva la cascada hacia el esclavo */
#define PIC_MASTER_IRQ 2

static void (*irq_routines[IRQ_COUNT])(void);

/* El EOI va despues del handler, si se manda antes, el PIC puede volver a
 * entregar la misma IRQ al instante (es el caso de las de nivel). */
static void pic_send_eoi(uint8_t irq) {
  if (irq >= PIC_RAW_BASE)
    outb(PIC2_CMD, PIC_EOI); /* primero el esclavo */

  outb(PIC1_CMD, PIC_EOI); /* y despues el maestro */
}

void irq_init(void) {
  /* ICW1: inicia, cascada presente, espera 4 bytes de ICW */
  outb(PIC1_CMD, PIC_INIT);
  outb(PIC2_CMD, PIC_INIT);

  /* ICW2: vectores. Maestro arranca en 32, esclavo en 40 */
  outb(PIC1_DATA, IRQ_BASE_VECTOR);
  outb(PIC2_DATA, IRQ_BASE_VECTOR + PIC_RAW_BASE);

  /* ICW3: la linea 2 del maestro es la cascada hacia el esclavo */
  outb(PIC1_DATA, PIC_MASTER_MASK);
  outb(PIC2_DATA, PIC_RAW_BASE);

  /* ICW4: modo 8086 */
  outb(PIC1_DATA, PIC_RAW_MODE);
  outb(PIC2_DATA, PIC_RAW_MODE);

  /* Por defecto dejar todo masked, cada driver desenmascara lo que necesita */
  outb(PIC1_DATA, 0xFF);
  outb(PIC2_DATA, 0xFF);
}

int irq_register(uint8_t irq, irq_handler_t handler) {
  if (irq >= IRQ_COUNT || handler == NULL)
    return -1;

  irq_routines[irq] = handler;
  return 0;
}

int irq_enable(uint8_t irq) {
  if (irq >= IRQ_COUNT)
    return -1;

  if (irq >= PIC_RAW_BASE) {
    /* bit del esclavo */
    uint8_t slave = inb(PIC2_DATA);
    slave &= ~(1 << (irq - PIC_RAW_BASE));
    outb(PIC2_DATA, slave);

    /* y la cascada (IRQ2 del maestro), si no el esclavo no llega al CPU */
    uint8_t master = inb(PIC1_DATA);
    master &= ~(1 << PIC_MASTER_IRQ);
    outb(PIC1_DATA, master);

    return 0;
  }

  uint8_t mask = inb(PIC1_DATA);
  mask &= ~(1 << irq);
  outb(PIC1_DATA, mask);

  return 0;
}

void irq_handler_c(struct regs *r) {
  uint32_t int_no = r->int_no;

  if (int_no < IRQ_BASE_VECTOR)
    return; /* no es una IRQ */

  uint8_t irq = (uint8_t)(int_no - IRQ_BASE_VECTOR);

  if (irq < IRQ_COUNT && irq_routines[irq] != NULL)
    irq_routines[irq]();

  pic_send_eoi(irq);
}
