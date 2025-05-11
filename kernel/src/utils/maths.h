#pragma once
#include <stddef.h>
#include <stdint.h>

#define DivRoundUp(number, divisor) ((number + divisor - 1) / divisor)

size_t align(size_t size, uint64_t alignment);

