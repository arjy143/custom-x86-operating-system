#include "paging.h"
#include "types.h"
#include "panic.h"
#include "pmm.h"
#include "libc.h"

//page directory and an identity-map page table
static page_directory_entry page_directory[ENTRIES_PER_TABLE] __attribute__((aligned(4096)));

static page_directory_entry first_page_table[ENTRIES_PER_TABLE] __attribute__((aligned(4096)));

//virtual address - first 10 bits are for directory, next 10 bits are for table, last 12 bits are for offset within table.

//get directory and table indices from virtual address
static uint32_t virt_to_dir_index(uint32_t virt)
{
    return (virt >> 22) & 0x3ff;
}

static uint32_t virt_to_table_index(uint32_t virt)
{
    return (virt >> 12) & 0x3ff;
}


//set up paging, involving setting up the directory, low memory maps, kernel mapping to higher half
void paging_init()
{
    //zero out variables
    uint32_t i;
    for (i = 0; i < ENTRIES_PER_TABLE; i++)
    {
        page_directory[i] = 0;
    }

    for (i = 0; i < ENTRIES_PER_TABLE; i++)
    {
        first_page_table[i] = 0;
    }

    //map first 4mb directly, so virtual == physical to keep things working for now
    for (i = 0; i < ENTRIES_PER_TABLE; i++)
    {
        uint32_t phys = i * PAGE_SIZE;

        first_page_table[i] = phys | PAGE_PRESENT | PAGE_WRITABLE;
    }

    //load first page table into directory entry 0
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;
    
    //load page directory into cr3 and set bit 31 of cr0 to enable paging
    __asm__ volatile
        (
            "mov %0, %%cr3"
            :
            : "r"((uint32_t)page_directory)
        );

    uint32_t cr0;

    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));

    cr0 |= 0x80000000;

    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

}

//map virtual to physical address
void paging_map(uint32_t virt, uint32_t phys, uint32_t flags)
{
    //extract the directory and table indices from the address
    uint32_t dir_index = virt_to_dir_index(virt);
    uint32_t table_index = virt_to_table_index(virt);

    //allocate new page_table if directory entry doesnt exist
    if (!(page_directory[dir_index] & PAGE_PRESENT))
    {
        uint32_t new_table = pmm_alloc_frame();
        memset((void*)new_table, 0, PAGE_SIZE);

        page_directory[dir_index] = new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }

    //get the page table address based on the directory
    page_table_entry* table = (page_table_entry*)(page_directory[dir_index] & 0xfffff000);

    table[table_index] = (phys & 0xfffff000) | flags | PAGE_PRESENT;
}
