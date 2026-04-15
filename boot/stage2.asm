[bits 16]
[org 0x0500]

start:
    mov [boot_drive], dl
    mov si, msg
.print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp .print
.done: 

    ; load kernel before gdt setup
    mov bx, 0x0000
    mov es, bx
    mov bx, 0x1000
    mov ah, 0x02
    mov al, 20
    mov ch, 0
    mov cl, 7
    mov dl, 0
    mov dl, [boot_drive]
    int 0x13

    ; fall through to gdt setup
    cli
    lgdt [gdt_descriptor] ; load gdt
    
    ; switch to protected mode by setting CR0 bit 0 to 1
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; flush the cpu pipeline by far jumping. this ensures that instructions prefetched in real mode are flushed
    jmp CODE_SEG:init_pm

gdt_start:

; we specify 3 entries, each 8 bytes.

; null segment for saftey
gdt_null:
    dd 0x00000000
    dd 0x00000000

; code segment
gdt_code:
    dw 0xffff ; max limit
    dw 0x0000 ; base address
    db 0x00 
    db 0x9a ; access bytes
    db 0xcf ; flags and more limit bits
    db 0x00 ; more base address bits

; data segment
gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 0x92
    db 0xcf
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10

msg db "stage 2 loading in progress...", 0x0d, 0x0a, 0

times 512 - ($ - $$) db 0

; now in 32 bit protected mode
[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; set up stack
    mov esp, 0x90000

    ; no more bios interrupts in protected mode. to print stuff, we can write to the vga buffer at 0xb8000
    ; dword is the size specifier to tell the cpu to write 4 bytes, whereas dw is a nasm specific instruction
    mov dword[0xb8000], 0x2f4b2f4f ; ok in vga
    hlt
