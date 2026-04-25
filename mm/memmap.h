#ifndef MEMMAP_H
#define MEMMAP_H

#include "types.h"

//struct for storing each 24 byte entry in memory map
struct memory_region
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed));

#define MEMORY_USABLE 1
#define MEMORY_RESERVED 2
#define MEMORY_ACPI 3
#define MEMORY_ACPI_NVS 4
#define MEMORY_BAD 5

#define MEMMAP_COUNT_ADDR 0x500
#define MEMMAP_ENTRIES_ADDR 0x504

//get number of entries
uint32_t memmap_count();

//get pointer to specific entry
struct memory_region* memmap_get(uint32_t i);

#endif
