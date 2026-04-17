#ifndef MEMORY_H
#define MEMORY_H

//init allocator
void memory_init();

//malloc
void* malloc(unsigned int size);

//find out how much memory is being used currently
unsigned int memory_used();

#endif
