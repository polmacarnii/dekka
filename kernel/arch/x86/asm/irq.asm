BITS 32

; Es la stub de IRQ, el PIC se remapea en el archivo irq.c a los vectores 32 a 47

extern irq_handler_c

section .text

%macro IRQ_NOERR 2
global irq%1_handler
irq%1_handler:
    push dword 0
    push dword %2
    jmp irq_common_stub
%endmacro

IRQ_NOERR 0, 32
IRQ_NOERR 1, 33
IRQ_NOERR 2, 34
IRQ_NOERR 3, 35
IRQ_NOERR 4, 36
IRQ_NOERR 5, 37
IRQ_NOERR 6, 38
IRQ_NOERR 7, 39
IRQ_NOERR 8, 40
IRQ_NOERR 9, 41
IRQ_NOERR 10, 42
IRQ_NOERR 11, 43
IRQ_NOERR 12, 44
IRQ_NOERR 13, 45
IRQ_NOERR 14, 46
IRQ_NOERR 15, 47

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    ; segmentos de datos del kernel (0x10) (ver gdt.c)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler_c
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popa

    ; quita err_code e int_no antes del iret
    add esp, 8
    iret
