## Build environment
### Windows
I would suggest to go with **WSL2** or even a **VM**. Please go to this [website](https://learn.microsoft.com/en-us/windows/wsl/install) to know how to install WSL2. <br>

### MacOS
I've never used MacOS before since I don't own a Mac. I know that MacOS is also unix-like so building the OS will be fine but disk creation won't work. <br> <br>
I would suggest using a **linux VM** or use **docker**. <br>
In the future, I will try to add MacOS support. <br>

### GNU/Linux
This is the OS that LightUX's build system supports. It is generally the best option for building LightUX. <br>

## Build Dependencies
- nasm
- make
- x86_64-elf-* (Unfortunately, I don't have an automatically build script yet.)
- parted
- fatlabel
- dosfstools
- git

## Installation
### Compiling the kernel and make disk
For building:
- Simply run: ``make kernel`` to only compile the kernel, it will be place in ``bin/kernel/kernel.bin``
- To actually build the disk and the full OS, run: ```make```
### Run
Run: ``make run``
However, ``make run`` will require QEMU and working graphics on your host

### Real hardware
> **WARNING:** Running this OS on real hardware might cause some damage. Do it at your own risk. <br>
``` sh
dd if=bin/disk.img of=/dev/sdX bs=1M
```

Please replace the ```/dev/sdX``` with the storage device that you want to install the OS on


