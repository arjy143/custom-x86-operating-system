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
    
    //this will call lidt in asm
    idt_load(&idtr);
}

//called for every interrupt
void isr_handler(unsigned int esp)
{
    unsigned char int_num = *((unsigned char*)(esp + 4));

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
