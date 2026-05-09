#ifndef PMM_H
#define PMM_H

#include "types.h"

//setting up the physical memory manager, which is a bitmap that stores the state of every 4kb frame in the 128mb of memory.
//this will be placed right after the kernel end address.

#define PMM_FRAME_SIZE 4096

void pmm_init();

//allocate a physical frame and get physical address
uint32_t pmm_alloc_frame();

//free physical memory
void pmm_free_frame(uint32_t frame_address);

uint32_t pmm_free_frames();

uint32_t pmm_used_frames();

#endif
