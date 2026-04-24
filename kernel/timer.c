#include "timer.h"
#include "io.h"

//channels live at 0x40,41,42
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43
#define PIT_BASE_FREQUENCY 1193182

static uint32_t ticks = 0;
static uint32_t seconds = 0;
static uint32_t hz = 0;

void timer_init(uint32_t frequency)
{
    hz = frequency;
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    
    /*
    0x36 = 0011 0110 
    00 channel 0 
    11 access mode: low byte then high byte 
    011 mode 3: square wave generator
    */
    port_out(PIT_COMMAND, 0x36);
    port_out(PIT_CHANNEL0, (uint8_t)(divisor & 0xff));
    port_out(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xff));
}

void timer_handler()
{
    ticks++;
    
    //TODO: instead of doing modulo, we could keep track of when the next second boundary should occur, which avoids dividing.
    if (ticks % hz == 0)
    {
        seconds++;
    }
}

uint32_t timer_get_ticks()
{
    return ticks;
}

uint32_t timer_get_seconds()
{
    return seconds;
}
