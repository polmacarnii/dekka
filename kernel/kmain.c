/* A este punto ya se tuvo que haber preparado todo lo que depende de la
 * arquiectura ya se resolvio a este punto :P */
#include <arch.h>
#include <dev/drivers.h>
#include <misc/colors.h>
#include <print.h>

void kmain(void) {

  drivers_init();
  print("Hello!\n");

  /* Hasta ahora ya que los drivers estan enganchados a sus IRQ, con las
   * interrupciones apagadas pues no llegan */
  interrupts_enable();

  for (;;) {
    halt();
  }
}
