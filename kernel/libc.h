#ifndef LIBC_H
#define LIBC_H

#include "types.h"

//bare minimum libc functions that could be useful for kernel level applications
void memset(void* ptr, uint8_t value, uint32_t size);

void memcpy(void* dest, void* src, uint32_t size);

int strlen(char* str); 

int strcmp(char* a, char* b);

void strcpy(char* dest, char* src);

void itoa(int32_t value, char* buf);

void strcat(char* dest, char* src);

#endif
