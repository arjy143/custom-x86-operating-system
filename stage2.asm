[bits 16]
[org 0x0500]

start:
    mov si, msg
.print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp .print
.done:
    hlt

msg db "loaded stage 2.", 0x0D, 0x0A, 0

