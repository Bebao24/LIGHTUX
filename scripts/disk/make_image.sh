#!/bin/bash

set -e

# $1 -> target system, $2 -> mount directory, $3 -> image path
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo "Please provide the correct arguments!"
    exit 1
fi

SCRIPT=$(realpath "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

if test -f /dev/loop101; then
    echo "/dev/loop101 is assigned for somethings"
    exit 1
fi

LIMINE_DIR="${SCRIPT_PATH}/../../bootloader/limine"
LIMINE_EXEC="${LIMINE_DIR}/limine"

## Check for the executable
if [ ! -f "${LIMINE_EXEC}" ]; then
    echo "Please compile the bootloader!"
    exit 1
fi

echo "Creating image..."
## 64 MB
dd if=/dev/zero of="${3}" bs=1M count=64

echo "Creating partitions..."

## One FAT32 partition
parted "${3}" mklabel msdos
parted "${3}" mkpart primary fat32 2048s 100%
parted "${3}" set 1 boot on
"$LIMINE_EXEC" bios-install "${3}" # Install limine

sudo losetup -P /dev/loop101 "${3}"

echo "Formatting image..."
sudo mkfs.fat -F 32 /dev/loop101p1
sudo fatlabel /dev/loop101p1 BOOT

echo "Mounting image..."
sudo mkdir -p "${2}"
sudo mount /dev/loop101p1 "${2}"

echo "Copying essential files..."
sudo mkdir -p "${2}/boot/limine" "${2}/boot/EFI/BOOT"
sudo cp "$LIMINE_DIR/limine-bios.sys" "${2}/boot/limine/"
sudo cp "$LIMINE_DIR/BOOTX64.EFI" "${2}/boot/EFI/BOOT"
sudo cp "$LIMINE_DIR/BOOTIA32.EFI" "${2}/boot/EFI/BOOT"

sudo cp -r $1/* $2/

## Perform clean up in cleanup.sh
CURRENT_DIRECTORY=$(dirname "$0")
chmod +x "${CURRENT_DIRECTORY}/cleanup.sh"
"${CURRENT_DIRECTORY}/cleanup.sh" "${2}"
