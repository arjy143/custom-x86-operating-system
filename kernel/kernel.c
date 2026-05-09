#include "idt.h"
#include "vga.h"
#include "keyboard.h"
#include "memory.h"
#include "types.h"
#include "shell.h"
#include "timer.h"
#include "memmap.h"

void kernel_main()
{
    vga_clear();
    vga_print(0,0, "Welcome to the kernel.", WHITE_ON_BLACK);

    memory_init();

    //ensure all memory regions are correctly registered
    memmap_supplement(); 

    idt_init();
    timer_init(100);    
    shell_init();

    //now the hardware can interrupt us
    __asm__ volatile ("sti");
    

    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
