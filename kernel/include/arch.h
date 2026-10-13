#ifndef ARCH_H
#define ARCH_H

#include <types.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb_t;

typedef struct {
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type;

} framebuffer_t;

void halt();

#endif
