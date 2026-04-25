[bits 16]
[org 0x0500]

start:
    mov [boot_drive], dl

    ; get memory map and store it at a specific address for use by the kernel
    xor ax, ax
    mov es, ax
    mov di, 0x504
    xor ebx, ebx
    mov edx, 0x534D4150 ; SMAP magic
    mov dword [0x500], 0
    mov cx, 0

.e820_loop:
    ; loop until all the memory regions have been retrieved
    mov eax, 0xE820
    mov ecx, 24
    int 0x15

    jc .e820_done

    mov ecx, 24

    cmp eax, 0x534D4150
    jne .e820_done

    xor ax, ax
    mov es, ax
    mov edx, 0x534D4150

    inc dword [0x500]
    add di, 24
    inc cx
    cmp cx, 20

    test ebx, ebx
    jz .e820_done
    jmp .e820_loop

.e820_done:
    xor ax, ax
    mov es, ax
    ; zero out everything
    xor ecx, ecx
    xor edx, edx
    xor ebx, ebx

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
    mov cl, 5
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    ; fall through to gdt setup
    cli
    lgdt [gdt_descriptor] ; load gdt
    
    ; switch to protected mode by setting CR0 bit 0 to 1
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; flush the cpu pipeline by far jumping. this ensures that instructions prefetched in real mode are flushed
    jmp CODE_SEG:init_pm

disk_error:
    mov si, err_msg
.print:
    lodsb
    test al, al
    jz .hang
    mov ah, 0x0E
    int 0x10
    jmp .print
.hang:
    hlt
    jmp .hang

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

boot_drive db 0
msg db "stage 2 loading in progress...", 0x0d, 0x0a, 0
err_msg db "disk error occured.", 0x0d, 0x0a, 0

times 1024 - ($ - $$) db 0

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
    
    ; copying kernel from 0x1000 to 0x100000
    mov esi, 0x1000
    mov edi, 0x100000
    mov ecx, 2560 ; dwords to copy, each is 4 bytes so about 20 sectors
    rep movsd ; repeat copying dword from esi to edi
    jmp 0x100000
    ; the jmp takes us to the kernel code now, so any code below this is redundant.

    ; no more bios interrupts in protected mode. to print stuff, we can write to the vga buffer at 0xb8000
    ; dword is the size specifier to tell the cpu to write 4 bytes, whereas dw is a nasm specific instruction
    ; mov dword[0xb8000], 0x2f4b2f4f ; ok in vga
    ; hlt
