
mov ah, 0x0e
mov al, the_secret
int 0x10
mov al, [the_secret]
int 0x10
mov al, the_secret + 0x7C00
int 0x10
mov al, 2d + 0x7C00
int 0x10

jmp $ ; infinite loop by jumping to current address

the_secret:
    db "X"

times 510-($-$$) db 0
dw 0xaa55
