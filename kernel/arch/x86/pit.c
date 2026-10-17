#include <arch.h>
#include <cpu.h>
#include <interrupts.h>
#include <pit.h>
#include <proc.h>
#include <types.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

/* El PIT cuenta a 1.193182 MHz (frecuencia del oscilador), el divisor que se
 * le carga es PIT_FREQUENCY / frecuencia_deseada */
#define PIT_FREQUENCY 1193182

/* Frecuencia del tick en Hz */
#define TIMER_HZ 100

#define PIT_CMD_CHANNEL0_SQUARE 0x36

/* Ticks desde el arranque. volatile porque lo escribe el handler de IRQ y lo
 * lee el resto del kernel */
static volatile uint32_t timer_ticks;

static void timer_tick(void) {
    timer_ticks++;
    need_resched = 1;
}

void timer_init(void) {
    uint32_t divisor = PIT_FREQUENCY / TIMER_HZ;

    /* El divisor se carga en dos bytes, primero el bajo y luego el alto, tal y
     * como pide el protocolo del 8254 */
    outb(PIT_COMMAND, PIT_CMD_CHANNEL0_SQUARE);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    /* Engancha el handler y desenmascara la IRQ0 en el PIC y a partir de aqui
     * llegan ticks en cuanto se habiliten las interrupciones globales */
    irq_register(0, timer_tick);
    irq_enable(0);
}

uint32_t timer_get_ticks(void) { return timer_ticks; }
