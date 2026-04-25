[bits 16] ; tells nasm to produce 16 bit code
[org 0x7C00] ; tells nasm where code will be when it runs, so any labels will now produce the correct absolute address

start:
    cli ; prevent interrupts during this sensitive set of operations
    xor ax, ax ; zero everything to ensure that when offsetting, we don't access the wrong memory
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti ; now allow interrupts again

    mov [boot_drive], dl ; save boot drive - could be hard drive ,usb, floppy

    ; stage 2 loading from disk
    mov bx, 0x0500 ; next free chunk of memory
    mov ah, 0x02 ; disk services: function to read sector
    mov al, 4 ; read n sectors
    
    ; cylinder, sector, head
    mov ch, 0
    mov cl, 2
    mov dh, 0

    mov dl, [boot_drive]
    int 0x13 ; disk services interrupt
    jc .disk_error ; if carry flag set, error occured

    jmp 0x0000:0x0500

.disk_error:
    mov si, error_msg
.print_loop:
    lodsb ; al is next character, so si now advances
    test al, al ; is al 0 - null terminator
    jz .hang ; if 0 then halt
    mov ah, 0x0e
    int 0x10
    jmp .print_loop
.hang:
    hlt
    jmp .hang

boot_drive db 0
error_msg db "Disk error", 0x0D, 0x0A, 0

; define bytes, with cr and lf and null terminator
times 510 - ($ - $$) db 0
dw 0xaa55
