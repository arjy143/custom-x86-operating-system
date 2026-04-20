#include "shell.h"
#include "vga.h"
#include "libc.h"
#include "memory.h"

//set colours green, white, red respectively
#define PROMPT_COLOUR 0x0a
#define OUTPUT_COLOUR 0x0f
#define ERROR_COLOUR 0x0c

static int cursor_row = 0;
static int cursor_col = 0;

static void newline()
{
    cursor_col = 0;
    cursor_row++;
    
    //now allow scrolling
    if (cursor_row >= VGA_ROWS)
    {
        vga_scroll();
        cursor_row = VGA_ROWS - 1;
    }
}

static void print(char* str, char colour)
{
    int i = 0;
    while (str[i] != 0)
    {
        //wrap around to next line
        if (cursor_col >= VGA_COLS)
        {
            newline();
        }

        vga_write_char(cursor_col, cursor_row, str[i], colour);
        cursor_col++;
        i++;
    }
}

static void println(char* str, char colour)
{
    print(str, colour);
    newline();
}

static void print_prompt()
{
    print("INPUT> ", PROMPT_COLOUR);
}


static void cmd_help()
{
    println("Available commands:", OUTPUT_COLOUR);
    println("help: this current message", OUTPUT_COLOUR);
    println("clear: clear screen", OUTPUT_COLOUR);
    println("mem: show memory usage", OUTPUT_COLOUR);
    println("echo <text>: print text to screen", OUTPUT_COLOUR);
}

static void cmd_clear()
{
    vga_clear();
    cursor_row = 0;
    cursor_col = 0;
}

static void cmd_mem()
{
    char buffer[32];
    itoa(memory_used(), buffer);
    print("Memory allocated: ", OUTPUT_COLOUR);
    print(buffer, OUTPUT_COLOUR);
    println("bytes.", OUTPUT_COLOUR);
}

static void cmd_echo(char* args)
{
    if (args == 0 || strlen(args) == 0)
    {
        newline();
    }
    else
    {
        println(args, OUTPUT_COLOUR);
    }
}

static void parse_and_run(char* input)
{
    int i = 0;
    while (input[i] == ' ')
    {
        i++;
    }

    char* cmd = input + i;

    //after trimming whitespace, if input is empty then ignore
    if (strlen(cmd) == 0)
    {
        return;
    }

    //parse command and args, and call corresponding inbuilt function
    int j = 0;
    while (cmd[j] != ' ' && cmd[j] != 0)
    {
        j++;
    }

    char* args = 0;
    if (cmd[j] == ' ')
    {
        cmd[j] = 0;
        args = cmd + j + 1;
    }

    if (strcmp(cmd, "help") == 0)
    {
        cmd_help();
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        cmd_clear();
    }
    else if (strcmp(cmd, "mem") == 0)
    {
        cmd_mem();
    }
    else if (strcmp(cmd, "echo") == 0)
    {
        cmd_echo(args);
    }
    else
    {
        print("Uknown command: ", ERROR_COLOUR);
        println(cmd, ERROR_COLOUR);
    }
}



//interface functions

void shell_init()
{
    vga_clear();
    println("=====Shell Test=====", PROMPT_COLOUR);
    println("commands: help; clear; mem; echo <arg>;", PROMPT_COLOUR);
    newline();
    print_prompt();
}

void shell_putchar(char c)
{
    vga_write_char(cursor_col, cursor_row, c, OUTPUT_COLOUR);
    cursor_col++;
    
    if (cursor_col >= VGA_COLS)
    {
        newline();
    }
}

void shell_backspace()
{
    if (cursor_col > 0)
    {
        cursor_col--;
        vga_write_char(cursor_col, cursor_row, ' ', OUTPUT_COLOUR);
    }
}

void shell_process(char* input)
{
    newline();
    parse_and_run(input);
    print_prompt();
}
