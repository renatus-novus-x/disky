#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#define FIXED_ONE (1024) // 1.0 sec = 1024
typedef uint16_t fixed_t;

#ifdef MSX

#define CLOCKS_PER_SEC (60)
#define JIFFY          (0xfc9e)

void platform_init(){
  uint8_t* ptr = (uint8_t*)JIFFY;
  ptr[0] = 0;
  ptr[1] = 0;
}

clock_t platform_clock(void) {
  uint8_t* ptr = (uint8_t*)JIFFY;
  return (clock_t)(ptr[0] | (ptr[1] << 8));
}

fixed_t platform_elapsed(clock_t start, clock_t end) {
  return (fixed_t)(((uint32_t)((end - start) & 0xFFFF) * FIXED_ONE) / CLOCKS_PER_SEC);
}
#else
#include <time.h>

#if X68K

#include <x68k/iocs.h>

void platform_init(){
//  _iocs_vdispst((void*)vbl_handler, 0, 1);
}

#undef  CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 100

static inline uint32_t trap_ontime_cs(void){
  uint32_t cs;
  __asm__ volatile(
    "moveq  #0x7F,%%d0 \n\t"  /* _ONTIME */
    "trap   #15        \n\t"  /* IOCS    */
    "move.l %%d0,%0    \n\t"
    : "=d"(cs)
    :
    : "d0","d1","a0","cc","memory"
  );
  return cs;
}

#endif

clock_t platform_clock(void) {
  return (clock_t)trap_ontime_cs();
}

/*
fixed_t platform_elapsed(clock_t start, clock_t end) {
  float sec = (float)(end - start) / CLOCKS_PER_SEC;
  return (fixed_t)(sec * (float)FIXED_ONE);
}
*/

float platform_elapsed_sec(clock_t start, clock_t end) {
  return (float)(end - start) / CLOCKS_PER_SEC;
}

#endif

#endif // PLATFORM_H
