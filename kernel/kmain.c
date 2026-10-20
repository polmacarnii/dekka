/* A este punto ya se tuvo que haber preparado todo lo que depende de la
 * arquiectura ya se resolvio a este punto :P */
#include <arch.h>
#include <dev/drivers.h>
#include <misc/colors.h>
#include <print.h>
#include <proc.h>

void kmain(void) {
    drivers_init();
    print("Hello!\n");

    interrupts_enable();
    /* Este contexto es el procs[0] */
    proc_init();

    /* procs[0] es el idle, asi que cuando no haynada listo se vuelve aqui y la
     * cpu espera interrupciones */
    for (;;) {
        sched();
        halt();
    }
}
