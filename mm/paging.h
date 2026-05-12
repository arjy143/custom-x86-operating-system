#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER 0x4

#define PAGE_SIZE 4096
#define ENTRIES_PER_TABLE 1024

typedef uint32_t page_directory_entry;
typedef uint32_t page_table_entry;

//set up paging, involving setting up the directory, low memory maps, kernel mapping to higher half
void paging_init();

//map virtual to physical address
void paging_map(uint32_t virt, uint32_t phys, uint32_t flags);

#endif
