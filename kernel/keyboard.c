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

//table to use when shift pressed
static char scancode_table_shift[128] = 
{
    0,    0,
    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
    '_',  '+',
    0,    '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',
    '{',  '}',
    0,    0,
    'A',  'S',  'D',  'F',  'G',  'H',  'J',  'K',  'L',
    ':',  '"',  '~',
    0,    '|',
    'Z',  'X',  'C',  'V',  'B',  'N',  'M',
    '<',  '>',  '?',
    0,    0,    0,    ' ',
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0
};

static int shift_held = 0;
static int caps_lock = 0;

static int cursor_col = 0;
static int cursor_row = 1;

//handle irq1
void keyboard_handler()
{
    //read scan code
    unsigned char scan_code = port_in(0x60);
    
    //check if shift is held or not
    if (scan_code == 0x2a || scan_code == 0x36)
    {
        shift_held = 1;
        return;
    }

    if (scan_code == 0xaa || scan_code == 0xb6)
    {
        shift_held = 0;
        return;
    }
    
    //check if caps lock is toggled, but ignore its release
    if (scan_code == 0x3a)
    {
        caps_lock = !caps_lock;
        return;
    }

    //ignore if released
    if (scan_code & 0x80)
    {
        return;
    }
    
    //get base character from normal table, and if caps lock is active or shift is pressed, switch to shift tabled
    char c = scancode_table[scan_code];

    //if the character is printable, print it
    if (c != 0)
    {
        //if its a letter, caps lock applies, but it doesnt apply for things like symbols or numbers.
        if (c >= 'a' && c <= 'z')
        {
            if (shift_held ^ caps_lock)
            {
                //ascii trick to get the capital letter mathematically
                c = c - 'a' + 'A';
            }
        }
        //if shift held, then use the shift table which applies to all keys, not just letters.
        else if (shift_held)
        {
            c = scancode_table_shift[scan_code];
        }

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
