include config/config.mk

.PHONY: all kernel bootloader disk clean run

all: disk

disk: $(BUILD_DIR)/disk.img

$(BUILD_DIR)/disk.img: kernel bootloader
	@ chmod +x scripts/disk/make_image.sh
	@ ./scripts/disk/make_image.sh target /mnt/lightux $@

kernel: always
	@ $(MAKE) -C kernel BUILD_DIR=$(abspath $(BUILD_DIR))

bootloader: always
	@ echo "Cloning limine"
	@ chmod +x ./bootloader/get_limine.sh
	@ ./bootloader/get_limine.sh

always:
	@ mkdir -p bin

run:
	qemu-system-x86_64 -drive file=bin/disk.img,format=raw -debugcon stdio -m 256M

clean:
	@ $(MAKE) -C kernel clean
	@ rm -rf bin/disk.img
