#ifndef PANIC_H
#define PANIC_H

//clear screen and show generic error
void kernel_panic(char* message) __attribute__((noreturn));

//display custom information before calling this, then halt
void kernel_halt() __attribute__((noreturn));
#endif
