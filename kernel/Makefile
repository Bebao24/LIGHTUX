TARGET_ASMFLAGS += -felf64
TARGET_CFLAGS += -Wall -Wextra -ffreestanding -fno-stack-protector -fno-stack-check \
				-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=large

TARGET_LINKFLAGS += -m elf_x86_64 \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -T linker.ld

## Include paths (Might need to make it seperately otherwise this might make a mess in the future)
TARGET_CFLAGS += -I ../libs -I ../libs/boot -I src/cpu -I src -I src/qemu -I src/drivers -I src/drivers/console -I src/drivers/framebuffer \
				-I src/utils -I src/utils/font -I src/utils/graphics -I src/arch/x86_64 -I src/arch/x86_64/cpu -I src/arch/x86_64/memory

C_SOURCES := $(shell find src -name "*.c")
C_OBJECTS := $(patsubst src/%.c, $(BUILD_DIR)/kernel/x86_64/%.o, $(C_SOURCES))

ASM_SOURCES := $(shell find src -name "*.asm")
ASM_OBJECTS := $(patsubst src/%.asm, $(BUILD_DIR)/kernel/x86_64/%_asm.o, $(ASM_SOURCES))

## TODO: Change this in the future
C_HEADERS := $(shell find src -name "*.h")
ASM_HEADERS := $(shell find src -name "*.inc")

TARGET_DIR = ../target

.PHONY: all kernel clean

all: kernel

kernel: $(TARGET_DIR)/boot/kernel.bin
$(TARGET_DIR)/boot/kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	@ mkdir -p $(TARGET_DIR)/boot
	$(TARGET_LD) $(TARGET_LINKFLAGS) -o $(BUILD_DIR)/kernel/kernel.bin $^ $(TARGET_LIBS)
	cp $(BUILD_DIR)/kernel/kernel.bin $@

$(BUILD_DIR)/kernel/x86_64/%.o: src/%.c $(C_HEADERS)
	@ mkdir -p $(@D)
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/kernel/x86_64/%_asm.o: src/%.asm $(ASM_HEADERS)
	@ mkdir -p $(dir $@)
	$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/kernel/*
	rm $(TARGET_DIR)/boot/kernel.bin
