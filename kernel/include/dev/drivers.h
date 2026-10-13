#ifndef DRIVER_H
#define DRIVER_H

#include <types.h>

struct driver {
  const char *name;
  const char *author;

  uint32_t version;

  int (*init)(void); /* La funcion principal del driver */
};

void driver_register(struct driver *drv);
void drivers_init();

#endif
