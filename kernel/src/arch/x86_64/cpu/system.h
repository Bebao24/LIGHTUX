#pragma once
#include <stdbool.h>

void panic(const char* message, ...);
void halt();

bool checkInterrupts();
