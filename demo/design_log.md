---
<b>1. Prereqs</b>
nasm, qemu, gcc, and also crosscompiler: sudo apt install -y gcc-i686-linux-gnu binutils-i686-linux-gnu 

---

<b>2. Basic bootloader</b>
Create bootloader. The BIOS will load the bootloader code, initially in 16 bit real mode, then switch to the 32 bit protected mode, and provide an entry point for the kernel.

Bootloader/MBR - master boot record - how BIOS hands off control, mem layout.

When it first boots, it checks the first 512 byte large sector, and looks for the magic bytes 0xAA55 at the end. This sector is then loaded into 0x7C00, where the BIOS jumps to. 

Historical reason - early IBM computers had 32kb of RAM, so an address with enough space above and below was needed. The below space would be used for the bootloader and IVT, the above space would be for the bootloader to load into.

<i>Real mode</i>

This is 16 bits, which means that we use the 16 bit registers in the CPU. This gives a hard ceiling of 2^16 bytes = 64kb memory, which is not enough when we need to load our whole kernel. 

To fix this, we use segmented addressing - use another 16 bit register for storing an offset. now, with an 0xFFFF segment register and 0xFFFF offset register, the max value possible is 0x10FFEF. Practically, the limit is 1MB, or 0xFFFFF.

| Dedicated registers for holding segment values:<br>CS — Code Segment (which segment your instructions live in) <br>DS — Data Segment (which segment your data lives in) <br>SS — Stack Segment (which segment your stack lives in) <br>ES — Extra Segment (general purpose, used in string operations) |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| <br>4 general purpose registers, each of which can be split into H and L:<br>AX - accumulator - maths, return values, io<br>BX - base - memory addressing<br>CX - counter - loops, shift counts<br>DX - data -io ports, wide mult<br><br>If AX contains 0x1234, AH has 12, AL has 34 - little endian   |
| <br>Other registers:<br>SI - source index - source data, string ops<br>DI - destination index<br>SP - stack pointer - top of stack, dont touch<br>BP - base pointer - reference stack in functions<br>IP - instruction pointer - never set directly                                                    |

```
xor ax, ax ; zeroes out register. more efficient since it doesn't require the literal 0x0000 to be made inside the instruction itself, and only needs to reference the register 2 times
```

cli, sti flags - clear interrupt flag - tell cpu to hold calls - used while doing sensitive things to prevent interrupts


---

<b>3. Load stage 2 bootloader from disk</b>

BIOS reads whole 512 byte sectors at a time. Sectors are identified using cylinder, head, sector coords - CHS. 1 indexed.
Bootloader is at 0,0,1. Stage 2 will be at 0,0,2.

int 0x13 - disk interrupt
int 0x10 - video interrupt

Important register values for when using disk interrupt:

| AH = 0x02 — function: read sectors <br>AL = N — number of sectors to read <br>CH = cylinder <br>CL = sector <br>DH = head<br>DL = drive — 0x00 = floppy, 0x80 = first hard disk <br>ES:BX = destination address in memory |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
Carry flag - single bit set by cpu. Clear means success. jc - jump if carry.

<i>Errors</i>
When we attempt to load the sector from disk, there may be an error, in which case we can jmp to a hlt loop. We do this instead of simply calling hlt once, because there are non-maskable interrupts that the cpu can trigger, that cli and sti wont affect. By looping hlt, we can ensure that hlt will always be the correct state.

We can use the dd command to clone data to disk

---
<b>4. Protected mode</b>
Limitations of real mode are only 16 bit registers, 1mb addressable memory, no memory protection, and no privilege levels. This is why protected mode is needed.

To switch, you can't just set a flag, you need a GDT - global descriptor table.
The reason for this is we can't rely on the old way of addressing, so we have a gdt that tells us where the segment lives and its properties. The code segment is always active, and must point to a valid segment, so if this isn't set in the gdt then the cpu will repeatedly fault - after 3 times the whole thing crashes.
In real mode, segment registers simply held a number that you could use together with the offset to get an address. For protected mode, we make segment registers hold an index to the gdt instead.

Each entry contains base address, limit, privilege level, and code/data flag (what is it used for). 3 minimum entries needed - null, code, and data segments.

> Bytes 0-1   Limit bits 0-15
> Bytes 2-4   Base address bits 0-23
> Byte  5     Access byte
> Byte  6     Flags + Limit bits 16-19
> Byte  7     Base address bits 24-31

Access byte contains a lot of useful info in each bit, such as present, privilege, descriptor, etc.

Segment registers no longer hold addresses, they hold a selector that encodes the gdt index, tabel indicator, and requested privilege.

Control registers - CR0-3, control fundamental cpu behaviour. CRO is 32 bits, where each controls an aspect of the CPU.

esp - extended stack pointer. Points to top of downward growing stack. Old one with sp is too close to bootloader, and too small for kernel.


---
<b>5. Making the kernel</b>

need a kernel entry point because C requires a valid stack, and the segments to be set, and the entry point to be first. The linker may not place kernel.c at the very start, so the entry point is needed.

A linker file is also needed. This specifies the entry point, and the location counter, which is the address the linker assumes the code will be loaded at - similar to doing org.

elf different to bin - elf comes with metadata like symbol tables, section headers, debug info etc. objcopy can be used to strip this metadata to leave the raw bytes.

<i>Summary so far</i>
Stage 1 bootloader - bios loads it to 0x7c00 in 16 bit real mode. This sets up segments, stack, saves boot drive num, loads stage 2 into 0x0500 using 0x13 bios command, and jumps to it.
Stage 2 bootloader - at 0x0500. loads kernel into 0x1000, sets up gdt, switches into protected mode, flushes cpu pipeline, sets up registers and bigger stack, copies kernel from 0x1000 to 0x100000, jumps to it.


---
<b>6. handling interrupts</b>
Previously we could trigger software interrupts like 0x10 or 0x13. For hardware interrupts like keys or mouse presses, we need an IDT -interrupt descriptor table.

We use the lidt instruction, just like lgdt. Each entry is 8 bytes like the gdt.

idt has up to 256 entries, first 32 are reserved by intel for things like divide by 0, debug, nmi, breakpoint, page fault, etc. 32-47 are available for hardware devices, 48-255 are custom.

>each entry looks like:
Bits 0-15    Lower 16 bits of handler function address
Bits 16-31   Segment selector (which code segment to use — 0x08, our kernel)
Bits 32-39   Zero (reserved)
Bits 40-47   Type and attributes
Bits 48-63   Upper 16 bits of handler function address

Type and attributes contains present, privilege, zero, gate type.

PIC - programmable interrupt controller - controls prioritising and handling multiple interrupts before sending their code to the cpu. CPU can only handle 1 interrupt at a time, so this is why we need to queue them at the PIC. It starts at 8 as the starting interrupt number for historical reasons, so we need to fix this.

Memory mapped IO - like how vga is mapped at 0xb8000. Hardware is able to intercept these specific addresses.
IO ports - separate mechanism. They don't share the memory address space, but instead have their own dedicated address space. 65536 ports from 0 to ffff. Requires 2 special instructions: in and out.
in reads 1 byte from a port to a register, out writes 1 byte from register to port.

The CPU has a dedicated pin that signals whether a transaction targets memory space or io space. mov targets memory, in and out target io.

The historical reason for having separate spaces was because memory space was limited at the time, separation was cleaner to them, protection was easier since the cpu can easily restrict programs. Only ring 0 kernel code can use in and out.

>Conventional assignments:
0x0020 - 0x0021     Master PIC 
0x00A0 - 0x00A1     Slave PIC
0x0040 - 0x0043     Programmable Interval Timer (PIT)
0x0060              Keyboard data
0x0064              Keyboard status/command
0x03F8              COM1 serial port
0x03D4 - 0x03D5     VGA control registers

2 addresses, 1 for commands, 1 for data. Command port is for operational instructions like initialisation, remapping interrupt numbers, or signaling that one has been handled. Data port is for reading or writing config data, like interrupt mask, controlling enabling or disabling of interrupts. Can also send follow up bytes using it.

We need to remap interrupt numbers because the cpu recognises its own interrupt numbers, from 0-32, which are reserved for intel specific ones. the rest up to 255 can be defined by us, so we can use those for the pic.

PIC consists of master and slave chip, each responsible for 8 interrupts, and the slave's output is connected into the master's irq2 input. We need to specify this.

We use the initialisation control words (ICWs).
ICW1 - defines operation mode
ICW2 - specifies interrupt vector address
ICW3 - specifies how the master and slave are connected
ICW4 - sets to 8086 mode

We also need to save the segment registers when an interrupt occurs because we want to ensure that the cpu can resume correctly again.

When an interrupt occurs, the cpu also pushes 3 things onto the stack: EFLAGS, CS, and EIP. It may also push an error code. We then further push an error code and interrupt number. Then we pusha which pushes all 8 general purpose registers, and then we push the 4 segment registers, and finally the isr handler function which takes the initial address of the stack, which lets us access the contents of the stack if we know the offsets.

<i>Summary so far</i>
IO port access - inline asm functions to talk to the pic and other hardware.
IDT - cpu directly reads this to know which functions to call when an interrupt happens.
PIC remapping - remap IRQs away from intel's reserved range.


---
<b>7. Keyboard driver</b>

When you press a key, the keyboard chip (8042) detects it and converts to a scan code. The chip then signals the pic through irq1, then the pic gives interrupt 33 to cpu, then cpu runs irq1 handler, then the handle runs the scan code from port 0x60 and converts it into a character that you can display on the screen.

Scan codes - each key has a number.

Currently we don't have scrolling, we just cycle back to the first column of the last line.


---
<b>8.memory management</b>

We need to implement some basic functions in ring 0 that can do stuff like libc. Of course we can't just use glibc or anything because they rely on OS syscalls that we don't have. We can make our own versions of basic functions like memcpy and malloc which can use a basic bump allocator for now, which we can improve upon later.

Linker file - we need to specify a kernel_end variable that will tell the compiler that its value is whatever the address the location counter is at after all the sections are placed. The symbol itself is the address, so we need to get the address of it in C.

Basic list of things to implement:
malloc, strcat, strcpy, memcpy, memset, itoa, strlen


---
<b>9. Shell</b>

Now we can make a basic shell, which will essentially consist of a buffer, and some preset commands that we can run.

Change keyboard driver to simply keep track of a buffer, which can be passed into the shell when needed.

---
<b>10. Future plans</b>

Paging
Better allocation
User space
Process scheduling
Filesystem
Basic networking
Text editor

---
<b>11. Timer driver</b>

If we want to implement paging, then we will need to keep track of time, so that we can have a timer interrupt to ensure no process hogs the CPU for too long. Also to ensure we keep track of the blocks of time allocated for processes.

We need to deal with the PIT - programmable interval timer. This chip fires IRQ0 at a configurable frequency.
3 channels. Channel 0 connects to IRQ0, so it's used for the system timer. Channel 1 was historically used for DRAM refresh, but it's not used now.
Channel 3 connects to the pc speaker.

To use channel 0, we provide a divisor value, the PIT counts down from there at 1.193.182 Hz, and when it reaches 0 is fires off IRQ0 and resets. The frequency value is a historical quirk of the original IBM pc's crystal.

1193 - 1000hz - 1ms per tick
11932 - 100 hz - 10ms per tick
65536 - 18.2hz - default bios setting

Channels live at 0x40-2. Mode register is 0x43. 
>Send to mode register:
0x36 = 0011 0110 
Bits 7-6 = 00 channel 0 
Bits 5-4 = 11 access mode: low byte then high byte 
Bits 3-1 = 011 mode 3: square wave generator 
Bit 0 = 0 binary counting

We can use the 1000 hz setting which means that 100 ticks means 1 second.

There will be some drift because 1193182 / 11931 = 99.9985 hz, so after 1 hour we would be off by 50ms. This would compound. This is why computers generally synchronise with an external clock, like using RTC clocks, or NTP servers.


---
<b>12. Paging</b>

We will use 4kb pages and frames, and 32 bit values where the first 10 bits are the page directory index, the next 1- are the page table index, and the last 12 are the offset within the page.

 Each entry in the page directory and page table is 4 bytes.
 > It has this layout:
 Bits 31-12    Physical address of the page table or frame
              (upper 20 bits — lower 12 are always zero since
               tables are 4KB aligned)
Bit 11-9      Available for OS use
Bit 8         Global
Bit 7         Page size (0 = 4KB)
Bit 6         Dirty (CPU sets this when page is written to)
Bit 5         Accessed (CPU sets this when page is read)
Bit 4         Cache disabled
Bit 3         Write through
Bit 2         User/supervisor (0 = kernel only, 1 = user accessible)
Bit 1         Read/write (0 = read only, 1 = read/write)
Bit 0         Present (1 = this entry is valid)

Higher half kernel - typically we reserve a section of memory just for the kernel. currently it lives at 1mb, in the lower half. We ideally would want it to take up the upper 1gb, and reserve the lower 3gb for user space stuff.

We need a physical memory manager that can keep track of free physical frames using bitmaps - 1 bit per frame, so for 4gb it would be 128kb.

>Our bitmap needs to mark the following areas as being used:
0x00000 - 0x004FF    IVT and BIOS data
0x00500 - 0x07BFF    Stage 2 bootloader
0x07C00 - 0x07DFF    Stage 1 bootloader
0x07E00 - 0x9FFFF    Free conventional memory
0xA0000 - 0xFFFFF    Hardware regions (VGA, BIOS ROM)
0x100000 - kernel_end    Kernel binary
kernel_end onwards   Free extended memory

Also, we need to find out how much RAM we have, which can be done using interrupt 0x15, eax = 0xe820, called in real mode. This returns a list of memory regions, where each entry is 24 bytes. It loops continuously by returning 1 entry each time and storing it in ebx, until the final entry which is 0. We can then store the e820 results at a bit of free memory for the kernel to use.

With seabios on qemu, using e820, we should find 6 memory regions. for some reason there are only 2. maybe e801?
