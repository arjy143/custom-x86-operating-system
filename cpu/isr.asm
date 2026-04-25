[bits 32]

; make this visible to C
global idt_load

extern isr_handler

idt_load:
    mov eax, [esp + 4] ; get pointer to idt_descriptor
    lidt [eax]
    ret

; not all of the cpu exceptions push error codes, so we can push 0 if one doesn't.

%macro ISR_NOERR 1
    global isr%1
    isr%1:
        ; push dummy error code 0 for interrupt number %1
        push byte 0
        push byte %1
        jmp isr_common
%endmacro

; error code already pushed in this case
%macro ISR_ERR 1
    global isr%1
    isr%1:
        push byte %1
        jmp isr_common
%endmacro

; CPU exceptions
ISR_NOERR 0     ; division by zero
ISR_NOERR 1     ; debug
ISR_NOERR 2     ; non maskable interrupt
ISR_NOERR 3     ; breakpoint
ISR_NOERR 4     ; overflow
ISR_NOERR 5     ; bound range exceeded
ISR_NOERR 6     ; invalid opcode
ISR_NOERR 7     ; device not available
ISR_ERR   8     ; double fault
ISR_NOERR 9     ; coprocessor segment overrun
ISR_ERR   10    ; invalid TSS
ISR_ERR   11    ; segment not present
ISR_ERR   12    ; stack segment fault
ISR_ERR   13    ; general protection fault
ISR_ERR   14    ; page fault
ISR_NOERR 15    ; reserved
ISR_NOERR 16    ; x87 floating point
ISR_ERR   17    ; alignment check
ISR_NOERR 18    ; machine check
ISR_NOERR 19    ; SIMD floating point
ISR_NOERR 20    ; virtualisation
ISR_NOERR 21    ; reserved
ISR_NOERR 22    ; reserved
ISR_NOERR 23    ; reserved
ISR_NOERR 24    ; reserved
ISR_NOERR 25    ; reserved
ISR_NOERR 26    ; reserved
ISR_NOERR 27    ; reserved
ISR_NOERR 28    ; reserved
ISR_NOERR 29    ; reserved
ISR_ERR   30    ; security exception
ISR_NOERR 31    ; reserved

; hardware IRQs for 32-47
%macro IRQ 2
    global irq%1
    irq%1:
        push byte 0
        push byte %2
        jmp isr_common
%endmacro

IRQ 0,  32 ; timer
IRQ 1,  33 ; keyboard
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; stack should contain various things at this point:
; [esp] - interrupt num
; [esp+4] - error code
; [esp+8] - eip
; [esp+12] - cs
; [esp+16] - eflags

isr_common:
    ; save all general purpose registers
    pusha

    ; save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; push pointer to stack as argument to C handler
    push esp
    call isr_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    ; restore general purpose registers
    popa
    add esp, 8
    ; return from interrupt. this also restores eflags and cs, to allow the cpu to resume correctly.
    iret
