#ifndef VGA_H
#define VGA_H

#include "types.h"

//vga buffer's location in protected mode
#define VGA_ADDRESS 0xb8000

#define VGA_COLS 80
#define VGA_ROWS 25
#define WHITE_ON_BLACK 0x0f

void vga_write_char(int32_t col, int32_t row, char c, int8_t colour);

void vga_clear();

void vga_print(int32_t col, int32_t row, char* str, int8_t colour);

void vga_scroll();

#endif
