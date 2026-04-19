#include "vga.h"

//pointer to buffer
//we use short because vga buffer consists of 2 byte pairs
unsigned short *vga = (unsigned short*)VGA_ADDRESS;

//write single char to specific position
void vga_write_char(int col, int row, char c, char colour)
{
    int index = row * VGA_COLS + col;
    vga[index] = (unsigned short)c | ((unsigned short)colour << 8);
}

void vga_clear()
{
    int i;
    for (i = 0; i < VGA_COLS * VGA_ROWS; i++)
    {
        vga[i] = (unsigned short) ' ' | ((unsigned short) WHITE_ON_BLACK << 8);
    }
}

//write whole string
void vga_print(int col, int row, char* str, char colour)
{
    int i = 0;
    while (str[i] != 0)
    {
        vga_write_char(col + i, row, str[i], colour);
        i++;
    }
}
