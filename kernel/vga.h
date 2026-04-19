#ifndef VGA_H
#define VGA_H

//vga buffer's location in protected mode
#define VGA_ADDRESS 0xb8000

#define VGA_COLS 80
#define VGA_ROWS 25
#define WHITE_ON_BLACK 0x0f

void vga_write_char(int col, int row, char c, char colour);

void vga_clear();

void vga_print(int col, int row, char* str, char colour);

#endif
