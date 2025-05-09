## Build environment
You should build this on a linux distribution.
I'm not sure if you can build this thing on macOS because loopback device won't work!
What about Windows? I suggest going with WSL2.

## Dependencies
- nasm
- make
- x86_64-elf-* (Unfortunately, I don't have an automatically build script yet.)
- QEMU (For emulation, can be tested on real hardware)
- parted
- fatlabel
- dosfstools

## Installation
### Compile the kernel and make disk
For building:
- Simply run: ``make kernel`` to only compile the kernel, it will be place in ``bin/kernel/kernel.bin``
- To actually build the disk and the full OS, run: ```make```
### Run
Run: ``make run``
However, ``make run`` require QEMU and working graphics on your host

### Real hardware
But what about real hardware? I think Limine will work on BIOS booting but for some reason, not on UEFI
Please run this: ``dd if=bin/disk.img of=/dev/sdX bs=1M``, replace /dev/sdX with your USB device


