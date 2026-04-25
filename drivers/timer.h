#ifndef TIMER_H
#define TIMER_H

#include "types.h"

//Initialise PIT to trigger at the given frequency, which could be 1193182Hz for a 10ms tick
void timer_init(uint32_t frequency);

void timer_handler();

uint32_t timer_get_ticks();

uint32_t timer_get_seconds();

#endif
