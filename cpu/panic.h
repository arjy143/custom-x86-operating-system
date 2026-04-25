#ifndef PANIC_H
#define PANIC_H

void kernel_panic(char* message) __attribute__((noreturn));

#endif
