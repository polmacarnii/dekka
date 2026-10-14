BITS 32

; stubs de excepciones
; las que el cpu empuja con codigo de error usan el dichoso ISR_ERR, las demas ISR_NOERR y empujan 0
extern exception_handler_c

section .text

%macro ISR_NOERR 1
global isr%1
isr%1:
    push dword 0
    push dword %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    push dword %1
    jmp isr_common_stub
%endmacro

isr_common_stub:
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
    call exception_handler_c
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popa

    ; quita err_code e int_no antes del iret
    add esp, 8
    iret

; #DE: division por cero
ISR_NOERR 0
; #DB: debug
ISR_NOERR 1
; NMI (interrupcion no enmascarable)
ISR_NOERR 2
; #BP: breakpoint
ISR_NOERR 3
; #OF: overflow
ISR_NOERR 4
; #BR: bound range exceeded
ISR_NOERR 5
; #UD: opcode invalido
ISR_NOERR 6
; #NM: device not available
ISR_NOERR 7
; #DF: double fault
ISR_ERR 8
; #TS: segment TSS invalido
ISR_NOERR 9
; #NP: segmento no presente
ISR_ERR 10
; #SS: error de pila
ISR_ERR 11
; #GP: error general de proteccion
ISR_ERR 12
; #PF: page fault
ISR_ERR 13
; reservado
ISR_ERR 14
; #MF: error de coma flotante
ISR_NOERR 15
; #AC: alineamiento no valido
ISR_ERR 16
; #MC: error de maquina
ISR_NOERR 17
; #XM: error de SIMD
ISR_NOERR 18
; cualquier vector libre, el CPU lo usa como "error de VMX"
ISR_NOERR 19
; #CP: error general de proteccion en modo virtual (VMX)
ISR_NOERR 20
; #CP: error de VMM en modo virtual
ISR_NOERR 21
; #CP: error de VMX en modo kernel
ISR_NOERR 22
; reserved
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
; #HV: error de hypervisor
ISR_NOERR 30
; #VC: error de comunicacion de VMM
ISR_NOERR 31
