/* carch = c arch */
/* es la funcion que se llama desde entry.asm */
/* el nombre carch no es muy bueno, probablemente sea cambiado */

#include "grub/multiboot.h"
#include <arch.h>
#include <dev/fb.h>
#include <dev/term.h>

void kmain(void);

static void hang(void) {
  for (;;)
    halt();
}

void carch(uint32_t magic, multiboot_info_t *bootinfo) {
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    hang();

  if (bootinfo->framebuffer_type != 1 || bootinfo->framebuffer_bpp != 32)
    hang();

  video_init((uint32_t *)(uintptr_t)bootinfo->framebuffer_addr,
             bootinfo->framebuffer_width, bootinfo->framebuffer_height,
             bootinfo->framebuffer_pitch);

  term_init();

  kmain();
}
