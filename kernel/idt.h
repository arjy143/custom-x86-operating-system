#ifndef IDT_H
#define IDT_H

//struct for 8 byte idt entry
/*
Bits 0-15    Lower 16 bits of handler function address
Bits 16-31   Segment selector (which code segment to use - 0x08, our kernel)
Bits 32-39   Zero (reserved)
Bits 40-47   Type and attributes
Bits 48-63   Upper 16 bits of handler function address
*/
struct idt_entry
{
    unsigned short base_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char attributes;
    unsigned short base_high;
} __attribute__((packed));

//pointed at by IDT
struct idt_descriptor
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

//declaring idt related functions

void idt_set_entry(int index, unsigned int base, unsigned short selector, unsigned char attributes);

void idt_init();

#endif
