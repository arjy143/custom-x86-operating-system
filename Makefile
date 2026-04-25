# configuration
AS = nasm
CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld
OBJCOPY = i686-linux-gnu-objcopy

# compiler and linker flags
CFLAGS = -m32 -ffreestanding -fno-pie -std=c11 -Wall -Wextra -Werror -O2 -I kernel -I drivers -I cpu -I mm -I lib -I shell
LDFLAGS = -m elf_i386 -T kernel/linker.ld

DISK = disk.img
SECTORS = 2880

# need to update this when new object files needed
KERNEL_OBJS = \
    kernel/kernel_entry.o \
    cpu/isr.o \
    cpu/idt.o \
    cpu/panic.o \
    drivers/vga.o \
    drivers/keyboard.o \
    drivers/timer.o \
    mm/memory.o \
    mm/memmap.o \
    lib/libc.o \
    shell/shell.o \
    kernel/kernel.o

all: $(DISK)

# disk image building. need to update the seek values if the size of the bootloader changes or the kernel sector changes
$(DISK): boot/boot.bin boot/stage2.bin kernel/kernel.bin
	dd if=/dev/zero of=$(DISK) bs=512 count=$(SECTORS)
	dd if=boot/boot.bin of=$(DISK) conv=notrunc bs=512 seek=0
	dd if=boot/stage2.bin of=$(DISK) conv=notrunc bs=512 seek=1
	dd if=kernel/kernel.bin of=$(DISK) conv=notrunc bs=512 seek=4

# $< refers to first dependency
# $@ refers to target name
# $^ refers to all dependencies
boot/boot.bin: boot/boot.asm
	$(AS) -f bin $< -o $@

boot/stage2.bin: boot/stage2.asm
	$(AS) -f bin $< -o $@

kernel/kernel_entry.o: kernel/kernel_entry.asm
	$(AS) -f elf $< -o $@

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

cpu/isr.o: cpu/isr.asm
	$(AS) -f elf $< -o $@

cpu/idt.o: cpu/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

cpu/panic.o: cpu/panic.c
	$(CC) $(CFLAGS) -c $< -o $@

drivers/vga.o: drivers/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

drivers/keyboard.o: drivers/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

drivers/timer.o: drivers/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

mm/memory.o: mm/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

mm/memmap.o: mm/memmap.c
	$(CC) $(CFLAGS) -c $< -o $@

lib/libc.o: lib/libc.c
	$(CC) $(CFLAGS) -c $< -o $@

shell/shell.o: shell/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

# link kernel
kernel/kernel.elf: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

kernel/kernel.bin: kernel/kernel.elf
	$(OBJCOPY) -O binary $< $@

# run qemu
run: $(DISK)
	qemu-system-i386 -drive format=raw,file=$(DISK),index=0,media=disk

# cleanup
clean:
	rm -f boot/boot.bin boot/stage2.bin
	rm -f kernel/kernel_entry.o kernel/kernel.o
	rm -f kernel/kernel.elf kernel/kernel.bin
	rm -f cpu/isr.o cpu/idt.o cpu/panic.o
	rm -f drivers/vga.o drivers/keyboard.o drivers/timer.o
	rm -f mm/memory.o mm/memmap.o
	rm -f lib/libc.o
	rm -f shell/shell.o
	rm -f $(DISK)

.PHONY: all run clean
