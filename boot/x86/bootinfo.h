#ifndef BOOTINFO_H
#define BOOTINFO_H

#include <types.h>

/* Esto deja en EAX, "TKBL" en LE*/
#define BOOT_MAGIC 0x544b424c

/* Entrada de memoria(mapa E820, INT 15h, EAX=0xE820) */
/* La estructura va empaquetada debido a que el bootloader lo escribe a mano con
 * offsets fijos de 24 bytes por entrada */
typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) e820_entry_t;

/* Informacion que pasa el bootloader al kernel */
typedef struct {
    uint32_t magic;        /* 0  BOOT_MAGIC */
    uint32_t version;      /* 4  version del protocolo */
    char bootloader[16];   /* 8  nombre del bootloader */
    uint32_t boot_drive;   /* 24 drive que dejo la BIOS */
    uint32_t mmap_count;   /* 28 numero de entradas E820 */
    uint32_t mmap_addr;    /* 32 puntero al arreglo e820_entry_t */
    uint64_t fb_addr;      /* 36 direccion fisica del framebuffer */
    uint32_t fb_pitch;     /* 44 bytes por linea */
    uint32_t fb_width;     /* 48 ancho en pixeles */
    uint32_t fb_height;    /* 52 alto en pixeles */
    uint32_t fb_bpp;       /* 56 bits por pixel */
    uint32_t fb_type;      /* 60 1 = color directo */
    uint32_t kernel_phys;  /* 64 direccion fisica de carga del kernel */
    uint32_t kernel_entry; /* 68 punto de entrada del kernel */
} __attribute__((packed)) boot_info_t;

#endif
