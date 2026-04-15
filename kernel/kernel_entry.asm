[bits 32]
extern kernel_main
global _start ; make visible to linker

_start:
    ; jump into the C code for the kernel
    call kernel_main
    hlt
