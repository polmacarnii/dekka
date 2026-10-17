/* A este punto ya se tuvo que haber preparado todo lo que depende de la
 * arquiectura ya se resolvio a este punto :P */
#include <arch.h>
#include <dev/drivers.h>
#include <misc/colors.h>
#include <print.h>
#include <proc.h>

void a(void) {
    for (;;)
        print_color(COLOR_BRIGHT_GREEN, "I'm A!\n");
}

void b(void) {
    for (;;)
        print_color(COLOR_BRIGHT_YELLOW, "I'm B!\n");
}

void c(void) {
    for (;;)
        print_color(COLOR_BRIGHT_MAGENTA, "I'm C!\n");
}

void kmain(void) {
    drivers_init();
    print("Hello!\n");

    /* Este contexto es el procs[0] */
    proc_init();
    kthread_create(a);
    kthread_create(b);
    kthread_create(c);

    interrupts_enable();

    /* procs[0] es el idle, asi que cuando no haynada listo se vuelve aqui y la
     * cpu espera interrupciones */
    for (;;) {
        sched();
        halt();
    }
}
