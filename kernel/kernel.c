#include "idt.h"
#include "vga.h"
#include "keyboard.h"
#include "memory.h"
#include "types.h"

void kernel_main()
{
    vga_clear();
    vga_print(0,0, "Welcome to the kernel.", WHITE_ON_BLACK);
    memory_init();

    idt_init();
    
    shell_init();

    //now the hardware can interrupt us
    __asm__ volatile ("sti");
    

    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
