/* Interfaz comun para los discos */
#ifndef DISK_H
#define DISK_H

#include <types.h>

typedef struct Disk Disk;
struct Disk {
    char name[16];
    uint secsize; /* 512, 4096 */
    uvlong nsec;
    long (*bio)(Disk *, int write, void *buf, uvlong lba, long n);
    int (*flush)(Disk *); /* puede ser nil */
    void *aux;            /* estado privado del driver */
};

#endif
