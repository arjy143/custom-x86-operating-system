#include "idt.h"
#include "vga.h"
#include "keyboard.h"
#include "memory.h"
#include "types.h"
#include "shell.h"
#include "timer.h"
#include "memmap.h"
#include "pmm.h"
#include "paging.h"
#include "task.h"

volatile uint32_t thread_a_count = 0;
volatile uint32_t thread_b_count = 0;

static void thread_a()
{
    for(;;)
    {
        thread_a_count++;
        task_yield();
    }
}

static void thread_b()
{
    for(;;)
    {
        thread_b_count += 2;
        task_yield();
    }
}

void kernel_main()
{
    vga_clear();
    vga_print(0,0, "Welcome to the kernel.", WHITE_ON_BLACK);

    //ensure all memory regions are correctly registered
    memmap_supplement(); 
    
    //use bump_alloc for pmm
    pmm_init();

    //set up heap and use malloc from now on
    memory_init();
    
    //set
    paging_init();

    idt_init();
    timer_init(100);    

    task_init();
    task_create(thread_a);
    task_create(thread_b);

    shell_init();

    //now the hardware can interrupt us
    __asm__ volatile ("sti");
    

    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
