#pragma once
#include <stddef.h>
#include <stdint.h>

#define DivRoundUp(number, divisor) ((number + divisor - 1) / divisor)
#define EXPORT_BYTE(target, first)                                             \
  ((first) ? ((target) & ~0xFF00) : (((target) & ~0x00FF) >> 8))
#define COMBINE_WORD(low, high) (((uint32_t)(high) << 16) | (uint16_t)(low))

#define min(a, b) ((a < b) ? (a) : (b))
#define max(a, b) ((a > b) ? (a) : (b))

size_t align(size_t size, uint64_t alignment);

