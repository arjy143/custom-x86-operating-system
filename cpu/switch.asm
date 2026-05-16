[bits 32]

global context_switch

; void context_switch(uint32_t* old_esp_ptr, uint32_t new_esp)
; save current registers on stack and store esp in old_esp_ptr.
; load new esp, restore registers, return into new task.

; [esp + 4] - old_esp_ptr
; [esp + 8] - new_esp

context_switch:
    ; save current registers
    push ebx
    push esi
    push edi
    push ebp

    ; save current esp
    mov eax, [esp + 20] ; 4 register pushes + old_esp_ptr = 20 bytes
    mov [eax], esp

    ; load new esp
    mov esp, [esp + 24]

    ; restore registers
    pop ebp
    pop edi
    pop esi
    pop ebx

    ; return into instruction pointer
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
    
