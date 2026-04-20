#include "vga.h"
#include "types.h"

//pointer to buffer
//we use short because vga buffer consists of 2 byte pairs
uint16_t *vga = (uint16_t*)VGA_ADDRESS;

//write single char to specific position
void vga_write_char(int32_t col, int32_t row, int8_t c, int8_t colour)
{
    int32_t index = row * VGA_COLS + col;
    vga[index] = (uint16_t)c | ((uint16_t)colour << 8);
}

void vga_clear()
{
    int32_t i;
    for (i = 0; i < VGA_COLS * VGA_ROWS; i++)
    {
        vga[i] = (uint16_t) ' ' | ((uint16_t) WHITE_ON_BLACK << 8);
    }
}

//write whole string
void vga_print(int32_t col, int32_t row, int8_t* str, int8_t colour)
{
    int32_t i = 0;
    while (str[i] != 0)
    {
        vga_write_char(col + i, row, str[i], colour);
        i++;
    }
}

void vga_scroll()
{
    int32_t row;
    int32_t col;

    //copy rows up by 1, then clear the last row
    for (row = 0; row < VGA_ROWS - 1; row++)
    {
        for (col = 0; col < VGA_COLS - 1; col++) 
        {
            int32_t dest = row * VGA_COLS + col;
            int32_t src = (row + 1) * VGA_COLS + col;
            vga[dest] = vga[src];
        }
    }

    for (col = 0; col < VGA_COLS; col++)
    {
        int32_t index = (VGA_ROWS - 1) * VGA_COLS + col;
        vga[index] = (uint16_t)' ' | ((uint16_t)WHITE_ON_BLACK << 8); 
    }
}
