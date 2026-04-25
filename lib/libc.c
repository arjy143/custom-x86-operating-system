#include "libc.h"
#include "types.h"

void memset(void* ptr, uint8_t value, uint32_t size)
{
    uint8_t* p = (uint8_t*)ptr;
    uint32_t i;
    
    for (i = 0; i < size; i++)
    {
        p[i] = value;
    }
}

void memcpy(void* dest, void* src, uint32_t size)
{
    uint8_t* d = (uint8_t*)dest;
    uint8_t* s = (uint8_t*)src;
    uint32_t i;
    
    for (i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}

int32_t strlen(char* str)
{
    int i = 0;
    
    while (str[i] != 0)
    {
        i++;
    }

    return i;
}

int32_t strcmp(char* a, char* b)
{
    int32_t i = 0;
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
    int32_t i = 0;

    while (src[i] != 0)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

void itoa(int32_t value, char* buf)
{
    if (value == 0)
    {
        buf[0] = '0';
        buf[1] = 0;
        
        return;
    }

    int32_t i = 0;
    int32_t negative = 0;
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

    int32_t j = 0;
    
    if (negative)
    {
        buf[j++] = '-';
    }

    int32_t k;

    for (k = i - 1; k >= 0; k--)
    {
        buf[j++] = temp[k];
    }
    buf[j] = 0;
}

void strcat(char* dest, char* src)
{
    int32_t i = strlen(dest);
    int32_t j = 0;

    while (src[j] != 0)
    {
        dest[i++] = src[j++];
    }
    dest[i] = 0;
}
