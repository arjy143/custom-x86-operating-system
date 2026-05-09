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

//manually add missing memory regions if only the first 2 are being found (conventional memory and bios data area)
void memmap_supplement()
{
    uint32_t count = memmap_count();
    struct memory_region* entries = (struct memory_region*)MEMMAP_ENTRIES_ADDR;

    //if extended mem region is missing
    uint32_t i;
    int has_extended = 0;

    for (i = 0; i < count; i++)
    {
        if (entries[i].base >= 0x100000 && entries[i].type == MEMORY_USABLE)
        {
            has_extended = 1;
            break;
        }
    }

    if (!has_extended)
    {
        //bios ROM
        entries[count].base = 0x0f0000;
        entries[count].length = 0x010000;
        entries[count].type = MEMORY_RESERVED;
        entries[count].acpi = 0;
        count++;

        //extended 127mb memory
        entries[count].base = 0x010000;
        entries[count].length = 0x7f00000;
        entries[count].type = MEMORY_USABLE;
        entries[count].acpi = 0;
        count++;

        //bios at top of address space
        entries[count].base = 0xfffc0000;
        entries[count].length = 0x40000;
        entries[count].type = MEMORY_RESERVED;
        entries[count].acpi = 0;
        count++;

        volatile uint32_t* c = (volatile uint32_t*)MEMMAP_COUNT_ADDR;
        *c = count;
    }
}

uint32_t memmap_total_usable()
{
    uint32_t total = 0;
    uint32_t count = memmap_count();
    
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        struct memory_region* r = memmap_get(i);

        if (r->type == MEMORY_USABLE)
        {
            total += (uint32_t)r->length;
        }
    }

    return total;
}

#pragma GCC diagnostic pop
