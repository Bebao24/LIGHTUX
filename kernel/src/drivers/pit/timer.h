#pragma once
#include <stdint.h>

extern uint64_t ticks;
extern uint32_t frequency;

void InitializeTimer();
void sleep(uint64_t miliseconds);

