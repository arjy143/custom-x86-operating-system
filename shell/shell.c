#include "shell.h"
#include "vga.h"
#include "libc.h"
#include "memory.h"
#include "types.h"
#include "timer.h"
#include "memmap.h"

//set colours green, white, red respectively
#define PROMPT_COLOUR 0x0a
#define OUTPUT_COLOUR 0x0f
#define ERROR_COLOUR 0x0c

static int32_t cursor_row = 0;
static int32_t cursor_col = 0;

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

static void print(char* str, int8_t colour)
{
    int32_t i = 0;
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

static void println(char* str, int8_t colour)
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
    println("uptime: show seconds since startup", OUTPUT_COLOUR);
    println("memmap: show memory regions", OUTPUT_COLOUR);
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
    println(" bytes.", OUTPUT_COLOUR);
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

static void cmd_uptime()
{
    char buf[32];
    itoa(timer_get_seconds(), buf);
    print(buf, OUTPUT_COLOUR);
    println(" seconds.", OUTPUT_COLOUR);
}

static void cmd_memmap()
{
    uint32_t count = memmap_count();
    char buffer[32];

    itoa(count, buffer);
    print("Memory regions: ", OUTPUT_COLOUR);
    println(buffer, OUTPUT_COLOUR);

    uint32_t i;
    for (i = 0; i < count; i++)
    {
        struct memory_region* r = memmap_get(i);

        print(" base: ", OUTPUT_COLOUR);
        itoa((uint32_t)r->base, buffer);
        print(buffer, OUTPUT_COLOUR);

        print(" length: ", OUTPUT_COLOUR);
        itoa((uint32_t)r->length, buffer);
        print(buffer, OUTPUT_COLOUR);

        print(" type: ", OUTPUT_COLOUR);
        switch (r->type)
        {
            case MEMORY_USABLE:
                println("Usable", 0x0a);
                break;
            case MEMORY_RESERVED:
                println("Reserved", 0x0a);
                break;
            case MEMORY_ACPI:
                println("ACPI", 0x0a);
                break;
            case MEMORY_ACPI_NVS:
                println("ACPI NVS", 0x0a);
                break;
            case MEMORY_BAD:
                println("Bad", 0x0a);
                break;
            default:
                println("Unknown", 0x0a);
                break;
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

static void cmd_rawmem() {
    uint8_t* p = (uint8_t*)0x500;
    char buffer[32];
    uint32_t i;
    for (i = 0; i < 100; i++) {
        itoa(p[i], buffer);
        print(buffer, OUTPUT_COLOUR);
        print(" ", OUTPUT_COLOUR);
        if ((i + 1) % 8 == 0) {
            newline();
        }
    }
    newline();
}
#pragma GCC diagnostic pop

static void parse_and_run(char* input)
{
    int32_t i = 0;
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
    int32_t j = 0;
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
    else if (strcmp(cmd, "uptime") == 0)
    {
        cmd_uptime();
    }
    else if (strcmp(cmd, "memmap") == 0)
    {
        cmd_memmap();
    }
    else if (strcmp(cmd, "rawmem") == 0)
    {
        cmd_rawmem();
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
    println("Input \"help\" to see possible commands.", PROMPT_COLOUR);
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
