#ifndef IO_H
#define IO_H

//write byte to io port. we use static inline to prevent an actual function call from being made, making it more efficient. we dont need the overhead of a function call here.
static inline void port_out(unsigned short port, unsigned char value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

//read byte from io port
static inline void port_in(unsigned short port)
{
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value): "Nd"(port));

    return value;
}

//in case of old hardware, we can induce a small delay using this harmless port read
static inline void io_wait()
{
    port_out(0x80, 0x00);
}
#endif
