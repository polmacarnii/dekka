; BIOS es quien carga esto y aqui solo guarda el drive de arranque y salta a stage2
BITS 16
org 0x7C00

%define BOOT_DRIVE_ADDR 0x0500
%define STAGE2_ADDR     0x7E00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00 ; la pila crece hacia abajo de 0x7C00

    ; BIOS deja en DL el drive de donde se arranco
    mov [BOOT_DRIVE_ADDR], dl
    jmp STAGE2_ADDR

    ; Relleno
    times 510-($-$$) db 0
    dw 0xAA55
