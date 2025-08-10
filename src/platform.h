#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#define FIXED_ONE (1024) // 1.0 sec = 1024
typedef uint16_t fixed_t;

#ifdef MSX
#define CLOCKS_PER_SEC 60
#define JIFFY          0xfc9e

clock_t platform_clock(void) {
  uint8_t* ptr = (uint8_t*)0xFC9E;
  return (clock_t)(ptr[0] | (ptr[1] << 8));
}

fixed_t platform_elapsed(clock_t start, clock_t end) {
  return (fixed_t)(((uint32_t)((end - start) & 0xFFFF) * FIXED_ONE) / CLOCKS_PER_SEC);
}
#else
#include <time.h>

clock_t platform_clock(void) {
  return clock();
}

fixed_t platform_elapsed(clock_t start, clock_t end) {
  float sec = (float)(end - start) / CLOCKS_PER_SEC;
  return (fixed_t)(sec * (float)FIXED_ONE);
}
#endif

#endif // PLATFORM_H
