#include "memory.h"
#include "types.h"

//linker defines this. This is the memory address after all the kernel binary
extern uint32_t kernel_end;

//We can make a simple bump allocator for now
static uint32_t heap = 0;

static uint32_t allocated = 0;

void memory_init()
{
    //we get the address because the address itself is the value. The linker doesn't allocate any storage for it
    heap = (uint32_t)&kernel_end;
}

void* malloc(uint32_t size)
{
    //first, align to 4 byte boundary for safe mem access
    //The CPU accesses 4 byte values most efficiently when address is divisible by 4, so we pad everything to be a multiple of 4 bytes.
    if (size % 4 != 0)
    {
        size += 4 - (size % 4);
    }

    //simple, just keep track of the next free chunk of memory and move the heap pointer along
    uint32_t address = heap;
    heap += size;
    allocated += size;

    return (void*)&address;
}

uint32_t memory_used()
{
    return allocated;
}
