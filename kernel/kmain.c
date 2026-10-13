/* A este punto ya se tuvo que haber preparado todo lo que depende de la
 * arquiectura ya se resolvio a este punto :P */
#include <arch.h>
#include <print.h>

void kmain(void) {
  print("Hello!\n");

  for (;;) {
    halt();
  }
}
