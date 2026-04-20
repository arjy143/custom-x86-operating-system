#ifndef SHELL_H
#define SHELL_H

#include "types.h"

void shell_init();

void shell_putchar(int8_t c);

void shell_backspace();

void shell_process(int8_t* input);

#endif
