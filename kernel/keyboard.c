#include "keyboard.h"
#include "vga.h"
#include "io.h"

//basic scan code lookup table, containing the outputs for each index

static char scancode_table[128] = 
{
    0,    //0x00 - nothing
    0,    //0x01 - escape
    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
    '-',  '=',
    0,    //0x0E - backspace
    '\t', //0x0F - tab
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',
    '[',  ']',
    0,    //0x1C - enter
    0,    //0x1D - left ctrl
    'a',  's',  'd',  'f',  'g',  'h',  'j',  'k',  'l',
    ';',  '\'', '`',
    0,    //0x2A - left shift
    '\\',
    'z',  'x',  'c',  'v',  'b',  'n',  'm',
    ',',  '.',  '/',
    0,    //0x36 - right shift
    0,    //0x37 - keypad *
    0,    //0x38 - left alt
    ' ',  //0x39 - space
    0,    //remaining keys - function keys, numpad etc
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0
};

static int cursor_col = 0;
static int cursor_row = 1;

//handle irq1
void keyboard_handler()
{
    //read scan code
    unsigned char scan_code = port_in(0x60);

    //ignore if released
    if (scan_code & 0x80)
    {
        return;
    }

    char c = scancode_table[scan_code];

    //if the character is printable, print it
    if (c != 0)
    {
        vga_write_char(cursor_col, cursor_row, c, WHITE_ON_BLACK);
        cursor_col++;

        //wrap
        if (cursor_col >= VGA_COLS)
        {
            cursor_col = 0;
            cursor_row++;
        }
    }
}
