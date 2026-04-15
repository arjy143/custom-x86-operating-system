#include "idt.h"
#include "io.h"

//the IDT data structure itself, holding 256 entries at 8 bytes each
struct idt_entry idt[256];
struct idt_descriptor idtr;


//set an idt entry, where the index is the interrupt number, and the base is the address of the handler function.
//selector will be code segment 0x08
void idt_set_entry(int index, unsigned int base, unsigned short selector, unsigned char attributes)
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
    idtr.base = (unsigned int)&idt;

    pic_remap();
    
    //initialise handlers for cpu exceptions
    idt_set_entry(0, (unsigned int)isr0, 0x08, 0x8e);
    idt_set_entry(1, (unsigned int)isr1, 0x08, 0x8e);
    idt_set_entry(2, (unsigned int)isr2, 0x08, 0x8e);
    idt_set_entry(3, (unsigned int)isr3, 0x08, 0x8e);
    idt_set_entry(4, (unsigned int)isr4, 0x08, 0x8e);
    idt_set_entry(5, (unsigned int)isr5, 0x08, 0x8e);
    idt_set_entry(6, (unsigned int)isr6, 0x08, 0x8e);
    idt_set_entry(7, (unsigned int)isr7, 0x08, 0x8e);
    idt_set_entry(8, (unsigned int)isr8, 0x08, 0x8e);
    idt_set_entry(9, (unsigned int)isr9, 0x08, 0x8e);
    idt_set_entry(10, (unsigned int)isr10, 0x08, 0x8e);
    idt_set_entry(11, (unsigned int)isr11, 0x08, 0x8e);
    idt_set_entry(12, (unsigned int)isr12, 0x08, 0x8e);
    idt_set_entry(13, (unsigned int)isr13, 0x08, 0x8e);
    idt_set_entry(14, (unsigned int)isr14, 0x08, 0x8e);
    idt_set_entry(15, (unsigned int)isr15, 0x08, 0x8e);
    idt_set_entry(16, (unsigned int)isr16, 0x08, 0x8e);
    idt_set_entry(17, (unsigned int)isr17, 0x08, 0x8e);
    idt_set_entry(18, (unsigned int)isr18, 0x08, 0x8e);
    idt_set_entry(19, (unsigned int)isr19, 0x08, 0x8e);
    idt_set_entry(20, (unsigned int)isr20, 0x08, 0x8e);
    idt_set_entry(21, (unsigned int)isr21, 0x08, 0x8e);
    idt_set_entry(22, (unsigned int)isr22, 0x08, 0x8e);
    idt_set_entry(23, (unsigned int)isr23, 0x08, 0x8e);
    idt_set_entry(24, (unsigned int)isr24, 0x08, 0x8e);
    idt_set_entry(25, (unsigned int)isr25, 0x08, 0x8e);
    idt_set_entry(26, (unsigned int)isr26, 0x08, 0x8e);
    idt_set_entry(27, (unsigned int)isr27, 0x08, 0x8e);
    idt_set_entry(28, (unsigned int)isr28, 0x08, 0x8e);
    idt_set_entry(29, (unsigned int)isr29, 0x08, 0x8e);
    idt_set_entry(30, (unsigned int)isr30, 0x08, 0x8e);
    idt_set_entry(31, (unsigned int)isr31, 0x08, 0x8e);
    
    //initialise handlers for pic interrupts
    idt_set_entry(32, (unsigned int)irq0, 0x08, 0x8e);
    idt_set_entry(33, (unsigned int)irq1, 0x08, 0x8e);
    idt_set_entry(34, (unsigned int)irq2, 0x08, 0x8e);
    idt_set_entry(35, (unsigned int)irq3, 0x08, 0x8e);
    idt_set_entry(36, (unsigned int)irq4, 0x08, 0x8e);
    idt_set_entry(37, (unsigned int)irq5, 0x08, 0x8e);
    idt_set_entry(38, (unsigned int)irq6, 0x08, 0x8e);
    idt_set_entry(39, (unsigned int)irq7, 0x08, 0x8e);
    idt_set_entry(40, (unsigned int)irq8, 0x08, 0x8e);
    idt_set_entry(41, (unsigned int)irq9, 0x08, 0x8e);
    idt_set_entry(42, (unsigned int)irq10, 0x08, 0x8e);
    idt_set_entry(43, (unsigned int)irq11, 0x08, 0x8e);
    idt_set_entry(44, (unsigned int)irq12, 0x08, 0x8e);
    idt_set_entry(45, (unsigned int)irq13, 0x08, 0x8e);
    idt_set_entry(46, (unsigned int)irq14, 0x08, 0x8e);
    idt_set_entry(47, (unsigned int)irq15, 0x08, 0x8e);


    //this will call lidt in asm
    idt_load(&idtr);
}

//called for every interrupt
void isr_handler(unsigned int esp)
{
    //this should get the interrupt number from the stack
    unsigned char int_num = *((unsigned char*)(esp + 48));

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
