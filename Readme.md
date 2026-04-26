# x86 Operating System

An x86 OS built from scratch, using nasm, gcc, and qemu. 

[Browser demo:](https://arjy143.github.io/x86-operating-system/demo/index.html)

# Screenshot

<img width="735" height="460" alt="image" src="https://github.com/user-attachments/assets/d88c51d1-57b8-405b-8713-713e9e0e00d2" />

# Architecture

- boot/: contains stage 1 and 2 bootloaders written in assembly
- kernel/: kernel and entrypoint
- cpu/: interrupt handling, GDT, panic
- drivers/: VGA, keyboard, timer drivers
- mm/: bump memory allocator, memory map
- libc/: simple set of libc functions, stdint
- shell/: shell commands

# Features

- 2 stage bootloader written in assembly
- BIOS E820 memory detection
- GDT
- 32 bit kernel
- VGA text buffer driver
- Keyboard driver
- Full IDT
- Timer driver
- Interactive shell that parses commands
- Memory allocator
- Minimal libc function implementations
- Terminal scrolling

# Building

To build from scratch, simply do:
```make run```

# Resources

These were useful references while building this:

- [OSDev Wiki](https://wiki.osdev.org)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [OS tutorial](https://github.com/cfenollosa/os-tutorial)

# To Do
- Physical memory manager for paging and virtual memory
- Process scheduling
- File system
