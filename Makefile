# configuration
AS = nasm
CC = i686-linx-gnu-gcc
LD = i686-linx-gnu-ld
OBJCOPY = i686-linx-gnu-objcopy

# compiler and linker flags
CFLAGS = -m32 -ffreestanding -fno-pie
LDFLAGS = -m elf_i386 -T kernel/linker.ld

DISK = disk.img
SECTORS = 2880

all: $(DISK)

# disk image building
$(DISK): boot/boot.bin boot/stage2.bin kernel/kernel.bin
	dd if=/dev/zero of=$(DISK) bs=512 count=$(SECTORS)
	dd if=boot/boot.bin of=$(DISK) conv=notrunc bs=512 seek=0
	dd if=boot/stage2.bin of=$(DISK) conv=notrunc bs=512 seek=1
	dd if=kernel/kernel.bin of=$(DISK) conv=notrunc bs=512 seek=6

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

# link kernel
kernel/kernel.elf: kernel/kernel_entry.o kernel/kernel.o
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
	rm -f $(DISK)

.PHONY: all run clean
