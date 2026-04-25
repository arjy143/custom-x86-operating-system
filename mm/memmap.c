#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

#include "memmap.h"
#include "vga.h"
#include "libc.h"

uint32_t memmap_count()
{   
    //We need to get the value of the int pointer at MEMMAP_COUNT_ADDR
    //The compiler won't like it, hence the need for disabling gcc warnings
    
    return *((uint32_t*)MEMMAP_COUNT_ADDR);
}

struct memory_region* memmap_get(uint32_t i)
{
    return ((struct memory_region*)MEMMAP_ENTRIES_ADDR) + i;
}

#pragma GCC diagnostic pop
