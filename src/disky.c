#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"

#define BLOCK_SIZE 256
#define FILE_NAME "BENCH.DAT"
#define TOTAL_SIZE (32 * 1024)
#define NUM_BLOCKS (TOTAL_SIZE / BLOCK_SIZE)
#define RAND_COUNT 128

char buffer[BLOCK_SIZE];

void fill_buffer(void) {
  for (int i = 0; i < BLOCK_SIZE; ++i)
    buffer[i] = (char)(i & 0xFF);
}

void write_seq(void) {
  FILE* fp = fopen(FILE_NAME, "wb");
  if (!fp) { printf("Write error\n"); return; }

  fill_buffer();
  for (int i = 0; i < NUM_BLOCKS; ++i)
    fwrite(buffer, 1, BLOCK_SIZE, fp);

  fclose(fp);
}

void read_seq(void) {
  FILE* fp = fopen(FILE_NAME, "rb");
  if (!fp) { printf("Read error\n"); return; }

  for (int i = 0; i < NUM_BLOCKS; ++i)
    fread(buffer, 1, BLOCK_SIZE, fp);

  fclose(fp);
}

void write_rand(void) {
  FILE* fp = fopen(FILE_NAME, "r+b");
  if (!fp) { printf("Random write error\n"); return; }

  fill_buffer();
  for (int i = 0; i < RAND_COUNT; ++i) {
    long pos = (rand() % NUM_BLOCKS) * BLOCK_SIZE;
    fseek(fp, pos, SEEK_SET);
    fwrite(buffer, 1, BLOCK_SIZE, fp);
  }

  fclose(fp);
}

void read_rand(void) {
  FILE* fp = fopen(FILE_NAME, "rb");
  if (!fp) { printf("Random read error\n"); return; }

  for (int i = 0; i < RAND_COUNT; ++i) {
    long pos = (rand() % NUM_BLOCKS) * BLOCK_SIZE;
    fseek(fp, pos, SEEK_SET);
    fread(buffer, 1, BLOCK_SIZE, fp);
  }

  fclose(fp);
}

void run_benchmark(const char* label, void (*func)(void), unsigned int size_bytes) {
  clock_t t1 = platform_clock();
  func();
  clock_t t2 = platform_clock();
  fixed_t sec = platform_elapsed(t1, t2);
  if (sec == 0) sec = 1;
  uint32_t rate_fixed = ((uint32_t)size_bytes * FIXED_ONE) / sec;
  uint16_t rate_int   = rate_fixed / FIXED_ONE;
  uint16_t rate_frac  = ((rate_fixed - rate_int * FIXED_ONE) * 100) / FIXED_ONE; // %02u -> x100
  uint16_t sec_int    = sec / FIXED_ONE;
  uint16_t sec_frac   = ((sec - sec_int * FIXED_ONE) * 1000) / FIXED_ONE;        // %03u ->x1000
  printf("%-10s: %u.%02u KB/s (%u.%03u sec)\n", label, rate_int, rate_frac, sec_int, sec_frac);
}

int main(int argc, char* argv[]) {
  printf("Disky - Disk Benchmark\n");
  printf("Target file: %s\n\n", FILE_NAME);

  run_benchmark("Seq Write", write_seq, TOTAL_SIZE);
  run_benchmark("Seq Read", read_seq, TOTAL_SIZE);
  run_benchmark("Rand Write", write_rand, RAND_COUNT * BLOCK_SIZE);
  run_benchmark("Rand Read", read_rand, RAND_COUNT * BLOCK_SIZE);

  printf("\nDone.\n");
  return 0;
}
