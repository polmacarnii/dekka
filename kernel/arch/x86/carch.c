/* carch = c arch */
/* es la funcion que se llama desde entry.asm */
/* el nombre carch no es muy bueno, probablemente sea cambiado */

#include <arch.h>
#include <bootinfo.h>
#include <dev/fb.h>
#include <dev/term.h>
#include <interrupts.h>

void kmain(void);

static void hang(void) {
  for (;;)
    halt();
}

void carch(uint32_t magic, boot_info_t *bootinfo) {
  if (magic != BOOT_MAGIC || bootinfo->magic != BOOT_MAGIC)
    hang();

  if (bootinfo->fb_type != 1 || bootinfo->fb_bpp != 32)
    hang();

  video_init((uint32_t *)(uintptr_t)bootinfo->fb_addr, bootinfo->fb_width,
             bootinfo->fb_height, bootinfo->fb_pitch);

  term_init();

  gdt_init();
  idt_init();
  irq_init();

  kmain();
}
