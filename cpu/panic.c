#include "panic.h"
#include "vga.h"

#define PANIC_COLOUR 0x4f

void kernel_panic(char* message)
{
    //disable interrupts while error being communicated
    __asm__ volatile ("cli");
    
    vga_clear();
    vga_print(0, 0, "KERNEL PANIC: ", PANIC_COLOUR);
    vga_print(14, 0, message, PANIC_COLOUR);
    vga_print(0, 2, "OS halted.", PANIC_COLOUR);

    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
