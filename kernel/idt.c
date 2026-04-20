#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "types.h"

//the IDT data structure itself, holding 256 entries at 8 bytes each
struct idt_entry idt[256];
struct idt_descriptor idtr;


//set an idt entry, where the index is the interrupt number, and the base is the address of the handler function.
//selector will be code segment 0x08
void idt_set_entry(int32_t index, uint32_t base, uint16_t selector, uint8_t attributes)
{
    idt[index].base_low = base & 0xFFFF;
    idt[index].base_high = (base >> 16) & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0; 
    idt[index].attributes = attributes;
    
}

//function to remap the PIC
//master maps to 32-39
//slave maps to 40-47
static void pic_remap()
{
    //ICW1
    port_out(0x20, 0x11);
    io_wait();

    port_out(0xA0, 0x11);
    io_wait();

    //ICW2
    //set new interrupt offsets as mentioned above
    port_out(0x21, 0x20); //32
    io_wait();

    port_out(0xA1, 0x28); //40
    io_wait();

    //ICW3
    //master slave connection on IRQ2
    port_out(0x21, 0x04);
    io_wait();

    port_out(0xA1, 0x02);
    io_wait();

    //ICW4
    //set 8086 mode
    port_out(0x21, 0x01);
    io_wait();

    port_out(0xA1, 0x01);
    io_wait();

    //unmask all IRQs
    port_out(0x21, 0x00);
    port_out(0xA1, 0x00);
}

//initialise idt by doing pic remap and loading idt register
void idt_init()
{
    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr.base = (uint32_t)&idt;

    pic_remap();
    
    //initialise handlers for cpu exceptions
    idt_set_entry(0, (uint32_t)isr0, 0x08, 0x8e);
    idt_set_entry(1, (uint32_t)isr1, 0x08, 0x8e);
    idt_set_entry(2, (uint32_t)isr2, 0x08, 0x8e);
    idt_set_entry(3, (uint32_t)isr3, 0x08, 0x8e);
    idt_set_entry(4, (uint32_t)isr4, 0x08, 0x8e);
    idt_set_entry(5, (uint32_t)isr5, 0x08, 0x8e);
    idt_set_entry(6, (uint32_t)isr6, 0x08, 0x8e);
    idt_set_entry(7, (uint32_t)isr7, 0x08, 0x8e);
    idt_set_entry(8, (uint32_t)isr8, 0x08, 0x8e);
    idt_set_entry(9, (uint32_t)isr9, 0x08, 0x8e);
    idt_set_entry(10, (uint32_t)isr10, 0x08, 0x8e);
    idt_set_entry(11, (uint32_t)isr11, 0x08, 0x8e);
    idt_set_entry(12, (uint32_t)isr12, 0x08, 0x8e);
    idt_set_entry(13, (uint32_t)isr13, 0x08, 0x8e);
    idt_set_entry(14, (uint32_t)isr14, 0x08, 0x8e);
    idt_set_entry(15, (uint32_t)isr15, 0x08, 0x8e);
    idt_set_entry(16, (uint32_t)isr16, 0x08, 0x8e);
    idt_set_entry(17, (uint32_t)isr17, 0x08, 0x8e);
    idt_set_entry(18, (uint32_t)isr18, 0x08, 0x8e);
    idt_set_entry(19, (uint32_t)isr19, 0x08, 0x8e);
    idt_set_entry(20, (uint32_t)isr20, 0x08, 0x8e);
    idt_set_entry(21, (uint32_t)isr21, 0x08, 0x8e);
    idt_set_entry(22, (uint32_t)isr22, 0x08, 0x8e);
    idt_set_entry(23, (uint32_t)isr23, 0x08, 0x8e);
    idt_set_entry(24, (uint32_t)isr24, 0x08, 0x8e);
    idt_set_entry(25, (uint32_t)isr25, 0x08, 0x8e);
    idt_set_entry(26, (uint32_t)isr26, 0x08, 0x8e);
    idt_set_entry(27, (uint32_t)isr27, 0x08, 0x8e);
    idt_set_entry(28, (uint32_t)isr28, 0x08, 0x8e);
    idt_set_entry(29, (uint32_t)isr29, 0x08, 0x8e);
    idt_set_entry(30, (uint32_t)isr30, 0x08, 0x8e);
    idt_set_entry(31, (uint32_t)isr31, 0x08, 0x8e);
    
    //initialise handlers for pic interrupts
    idt_set_entry(32, (uint32_t)irq0, 0x08, 0x8e);
    idt_set_entry(33, (uint32_t)irq1, 0x08, 0x8e);
    idt_set_entry(34, (uint32_t)irq2, 0x08, 0x8e);
    idt_set_entry(35, (uint32_t)irq3, 0x08, 0x8e);
    idt_set_entry(36, (uint32_t)irq4, 0x08, 0x8e);
    idt_set_entry(37, (uint32_t)irq5, 0x08, 0x8e);
    idt_set_entry(38, (uint32_t)irq6, 0x08, 0x8e);
    idt_set_entry(39, (uint32_t)irq7, 0x08, 0x8e);
    idt_set_entry(40, (uint32_t)irq8, 0x08, 0x8e);
    idt_set_entry(41, (uint32_t)irq9, 0x08, 0x8e);
    idt_set_entry(42, (uint32_t)irq10, 0x08, 0x8e);
    idt_set_entry(43, (uint32_t)irq11, 0x08, 0x8e);
    idt_set_entry(44, (uint32_t)irq12, 0x08, 0x8e);
    idt_set_entry(45, (uint32_t)irq13, 0x08, 0x8e);
    idt_set_entry(46, (uint32_t)irq14, 0x08, 0x8e);
    idt_set_entry(47, (uint32_t)irq15, 0x08, 0x8e);


    //this will call lidt in asm
    idt_load(&idtr);
}

//called for every interrupt
void isr_handler(uint32_t esp)
{
    //this should get the interrupt number from the stack
    uint8_t int_num = *((uint8_t*)(esp + 48));
    
    if (int_num == 33)
    {
        keyboard_handler();
    }

    if (int_num >= 32)
    {
        if (int_num >= 40)
        {
            //End of interrupt signal to slave
            port_out(0xA0, 0x20);
        }
        //EOI to master
        port_out(0x20, 0x20);
    }
}
