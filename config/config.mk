export CFLAGS = -std=c99 -g
export ASMFLAGS =
export CC = gcc
export CXX = g++
export LD = gcc
export ASM = nasm
export LINKFLAGS =
export LIBS =

export TARGET = x86_64-elf
export TARGET_ASM = nasm
export TARGET_ASMFLAGS =
export TARGET_CFLAGS = -std=gnu99 -g
export TARGET_CC = $(TARGET)-gcc
export TARGET_CXX = $(TARGET)-g++
export TARGET_LD = $(TARGET)-ld
export TARGET_LINKFLAGS =
export TARGET_LIBS =

export BUILD_DIR = $(abspath bin)
