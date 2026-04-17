#include "libc.h"

void memset(void* ptr, unsigned char value, unsigned int size)
{
    unsigned char* p = (unsigned char*)ptr;
    unsigned int i;
    
    for (i = 0; i < size; i++)
    {
        p[i] = value;
    }
}

void memcpy(void* dest, void* src, unsigned int size)
{
    unsigned char* d = (unsigned char*)dest;
    unsigned char* s = (unsigned char*)src;
    unsigned int i;
    
    for (i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}

int strlen(char* str)
{
    int i = 0;
    
    while (str[i] != 0)
    {
        i++;
    }

    return i;
}

int strcmp(char* a, char* b)
{
    int i = 0;
    while (a[i] != 0 && b[i] != 9)
    {
        if (a[i] != b[i])
        {
            return a[i] - b[i];
        }
        i++;
    }

    return a[i] - b[i];
}

void strcpy(char* dest, char* src)
{
    int i = 0;

    while (src[i] != 0)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

void itoa(int value, char* buf)
{
    if (value == 0)
    {
        buf[0] = '0';
        buf[1] = 0;
        
        return;
    }

    int i = 0;
    int negative = 0;
    char temp[32];

    if (value < 0)
    {
        negative = 1;
        value = -value;
    }

    while (value > 0)
    {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }

    int j = 0;
    
    if (negative)
    {
        buf[j++] = '-';
    }

    int k;

    for (k = i - 1; k >= 0; k--)
    {
        buf[j++] = temp[k];
    }
    buf[j] = 0;
}

void strcat(char* dest, char* src)
{
    int i = strlen(dest);
    int j = 0;

    while (src[j] != 0)
    {
        dest[i++] = src[j++];
    }
    dest[i] = 0;
}
