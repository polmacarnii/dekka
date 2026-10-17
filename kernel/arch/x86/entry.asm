BITS 32; Modo protegido

; Aqui empieza grub a buscar
section .multiboot
align 4

MBALIGN equ 1<<0 ; Info alineada correctamente en memoria
MEMINFO equ 1<<1 ; Pedir memoria
VIDEO equ 1<<2 ; Pedir video


FLAGS    equ MBALIGN | MEMINFO | VIDEO
MAGIC    equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

dd MAGIC
dd FLAGS
dd CHECKSUM

; campos no usados
dd 0
dd 0
dd 0
dd 0
dd 0

; framebuffer
dd 0 ; mode type: 0 = graficos
dd 800 ; 800
dd 600; 600
dd 32; 32 bits de color

; Pila
section .bss
align 16

stack_bottom:
    resb 32768      ; 32 KiB

stack_top:

; codigo
section .text
global kernel_entry
extern carch

kernel_entry:
    cli

    mov esp, stack_top ; esto inicia la pila

    and esp, -16 ; alinear a 16 bytes

    ; Parametros a carch()
    ; EAX = magic
    ; EBX = boot_info_t*
    push ebx
    push eax

    call carch

.hang:
    cli
    hlt
    jmp .hang
