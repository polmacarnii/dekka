/* Handler de excepciones de la CPU */

#include <interrupts.h>
#include <klib/panic.h>
#include <misc/colors.h>
#include <print.h>

static const char *exception_names[32] = {"Divide Error",
                                          "Debug",
                                          "NMI",
                                          "Breakpoint",
                                          "Overflow",
                                          "Bound Range Exceeded",
                                          "Invalid Opcode",
                                          "Device Not Available",
                                          "Double Fault",
                                          "Coprocessor Segment Overrun",
                                          "Invalid TSS",
                                          "Segment Not Present",
                                          "Stack Fault",
                                          "General Protection Fault",
                                          "Page Fault",
                                          "Reserved",
                                          "FPU Exception",
                                          "Alignment Check",
                                          "Machine Check",
                                          "SIMD FP Exception",
                                          "Virtualization",
                                          "Control Protection",
                                          "Reserved",
                                          "Reserved",
                                          "Reserved",
                                          "Reserved",
                                          "Reserved",
                                          "Reserved",
                                          "Hypervisor Injection",
                                          "VMM Communication",
                                          "Security",
                                          "Reserved"};

void exception_handler_c(struct regs *r) {
  uint32_t int_no = r->int_no;

  print_colorf(COLOR_RED, "\n[EXCEPTION] ");

  if (int_no < ISR_COUNT)
    print(exception_names[int_no]);
  else
    print("Unknown exception");

  print_fmt(" (vector %d, code error %d)\n", (int32_t)int_no,
            (int32_t)r->err_code);

  /* No hay TSS ni cambio de pila todavia, asi que no hay de donde volver */
  panic("Unhandled exception");
}
