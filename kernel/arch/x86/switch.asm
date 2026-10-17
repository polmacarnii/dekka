; void swtch(uchar **old, uchar *new);
global swtch
section .text
swtch:
    mov eax, [esp + 4]
    mov edx, [esp + 8]

    push ebp
    push ebx
    push esi
    push edi

    mov [eax], esp ; Guarda el stack pointer del proceso actual
    mov esp, edx ; Cambia al stack del nuevo proceso

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
