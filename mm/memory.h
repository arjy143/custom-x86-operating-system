#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

//init allocator
void memory_init();

//malloc
void* malloc(uint32_t size);

//find out how much memory is being used currently
uint32_t memory_used();

#endif
