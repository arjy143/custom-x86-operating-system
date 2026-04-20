#ifndef LIBC_H
#define LIBC_H

#include "types.h"

//bare minimum libc functions that could be useful for kernel level applications
void memset(void* ptr, uint8_t value, uint32_t size);

void memcpy(void* dest, void* src, uint32_t size);

int strlen(int8_t* str); 

int strcmp(int8_t* a, int8_t* b);

void strcpy(int8_t* dest, int8_t* src);

void itoa(int32_t value, int8_t* buf);

void strcat(int8_t* dest, int8_t* src);

#endif
