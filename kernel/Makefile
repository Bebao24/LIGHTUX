TARGET_ASMFLAGS += -felf64
TARGET_CFLAGS += -Wall -Wextra -ffreestanding -fno-stack-protector -fno-stack-check \
				-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=large

TARGET_LINKFLAGS += -m elf_x86_64 \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -T linker.ld

## Include paths
TARGET_CFLAGS += -I ../libs -I ../libs/boot -I src/cpu -I src

C_SOURCES := $(shell find src -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/kernel/x86_64/%.o, $(C_SOURCES))

## TODO: Change this in the future
C_HEADERS := $(shell find src -name "*.h")

TARGET_DIR = ../target

.PHONY: all kernel clean

all: kernel

kernel: $(TARGET_DIR)/boot/kernel.bin
$(TARGET_DIR)/boot/kernel.bin: $(C_OBJECTS)
	@ mkdir -p $(TARGET_DIR)/boot
	$(TARGET_LD) $(TARGET_LINKFLAGS) -o $(BUILD_DIR)/kernel/kernel.bin $^ $(TARGET_LIBS)
	cp $(BUILD_DIR)/kernel/kernel.bin $@

$(BUILD_DIR)/kernel/x86_64/%.o: src/%.c $(C_HEADERS)
	@ mkdir -p $(@D)
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/kernel/*
	rm $(TARGET_DIR)/boot/kernel.bin
