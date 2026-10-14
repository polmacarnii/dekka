#ifndef PRINT_H
#define PRINT_H

#include <arch.h>
#include <arg.h>
#include <types.h>

/* Escribe texto plano en la terminal del kernel */
void print(const char *s);

/* Escribe un unico caracter */
void printc(char c);

/* Escribe un numero entero con signo en base 10 */
void print_dec(int32_t n);

/* Escribe un numero sin signo en base 10 */
void print_uint(uint32_t n);

/* Escribe un numero en base 16 precedido por "0x" */
void print_hex(uint32_t n);

/* Escribe un puntero en hexadecimal precedido por "0x" */
void print_ptr(const void *p);

/* Escribe texto con formato estilo printf.
 * Soporta: %s %c %d %i %u %x %X %p %% */
void print_fmt(const char *fmt, ...);
void vprint_fmt(const char *fmt, va_list ap);

/* Escribe texto en el color indicado y despues restaura el que habia. Solo
 * cambia el foreground y el fondo se deja como estaba */
void print_color(rgb_t fg, const char *s);
void print_colorf(rgb_t fg, const char *fmt, ...);

#endif
