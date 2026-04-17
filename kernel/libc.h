#ifndef LIBC_H
#define LIBC_H

//bare minimum libc functions that could be useful for kernel level applications
void memset(void* ptr, unsigned char value, unsigned int size);

void memcpy(void* dest, void* src, unsigned int size);

int strlen(char* str); 

int strcmp(char* a, char* b);

void strcpy(char* dest, char* src);

void itoa(int value, char* buf);

void strcat(char* dest, char* src);

#endif
