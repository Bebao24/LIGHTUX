## Build environment
You should build this on a linux distribution.
I'm not sure if you can build this thing on macOS because loopback device won't work!
What about Windows? I suggest going with WSL2.

## Dependencies
- NASM
- MAKE
- x86_64-elf-* (Unfortunately, I don't have an automatically build script yet.)
- QEMU (For emulation, can be tested on real hardware)
- GIT
- PARTED
- FATLABEL

## Installation
### Compile the kernel and make disk
Simply run: ``make`` or ``make kernel`` to only compile the kernel, it will be place in ``bin/kernel/kernel.bin``
### Run
Run: ``make run``
However, ``make run`` require QEMU and working graphics on your host

### Real hardware
But what about real hardware? I think Limine will work on BIOS booting but for some reason, not UEFI booting
Just run this: ``dd if=bin/disk.img of=/dev/sdX bs=1M``, please replace /dev/sdX with your USB device


