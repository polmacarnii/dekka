BITS 16
org 0x7E00

; Mapa de memoria que reservaa el bootloader, nada debe solaparse con la imagen de arranue(0x7C00), la pila (para abajo de 0x77C00) ni el staging del ELF (0x10000+)
%define BOOT_DRIVE_ADDR  0x0500
%define BOOT_INFO_ADDR   0x2000
%define E820_ADDR        0x2800
%define E820_MAX         32
%define VBE_CTRL_ADDR    0x3000
%define VBE_MODE_ADDR    0x3200
%define SECTOR_BUF       0x4000
%define DIR_BUF          0x4800
%define STACK_TOP        0x7C00
%define PM_STACK_TOP     0x9F000

; El kernel se lee aqui antes de parsearlo y copiarlo
%define KERNEL_STAGE_ADDR 0x10000
%define KERNEL_STAGE_MAX  0x70000 ; ~448 KiB, por debajo de la pila de PM

%define BOOT_MAGIC 0x544b424c ; "TKBL"

; Hay ciertas BIOS que no conservan DS/ES al volver de una INT
; Por eso, esta magro los devuelve a cero preservando AX y las flags
%macro FIXDS 0
    push ax
    pushf
    xor ax, ax
    mov ds, ax
    mov es, ax
    popf
    pop ax
%endmacro

; EMPIEZA LO BUENO!
stage2:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, STACK_TOP

    mov dl, [BOOT_DRIVE_ADDR]
    mov [boot_drive], dl

    mov si, msg_banner
    call puts
    
    ; solo llamar funciones
    call check_edd
    call enable_a20
    call detect_memory
    call set_vbe
    call load_kernel
    call enter_pm ; esta no retorna

; Funciones
puts:
    push ax
    push bx
    push si
    push bp
    mov bp, si
.next:
    mov al, [bp]
    inc bp
    test al, al
    jz .done
    mov ah, 0x0E
    mov bx, 0x0007
    push bp
    int 0x10
    pop bp
    FIXDS
    jmp .next
.done:
    pop bp
    pop si
    pop bx
    pop ax
    ret

; panic solo vuelve a modo texto y muestra el mensaje en DS:SI
panic:
    push si
    mov ax, 0x0003
    int 0x10
    xor ax, ax
    mov ds, ax
    mov es, ax
    pop si
    call puts
hang:
    cli
    hlt
    jmp hang


; checa que existan las extenciones INT 13h
check_edd:
    mov dl, [boot_drive]
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13
    FIXDS
    jc .fail
    cmp bx, 0xAA55
    jne .fail
    ret
.fail:
    mov si, msg_no_edd
    call panic

; 0x92: puerto rapido + controlador de teclado
enable_a20:
    in al, 0x92
    test al, 2
    jnz .done
    or al, 2
    and al, 0xFE ; no disparar el reset del bit 0
    out 0x92, al

    call kbc_wait
    mov al, 0xD1
    out 0x64, al
    call kbc_wait
    mov al, 0xDF ; bit 1 = A20 on
    out 0x60, al
.done:
    ret

kbc_wait:
    in al, 0x64
    test al, 2
    jnz kbc_wait
    ret

; detectar memoria: mapa E820 (int 15h) -> E820_ADDR y cuenta en e820_count
detect_memory:
    xor ebx, ebx
    xor bp, bp
    mov di, E820_ADDR
.loop:
    mov eax, 0xE820
    mov edx, 0x534D4150 ; 'SMAP'
    mov ecx, 24
    mov dword [es:di+20], 0
    int 0x15
    FIXDS
    jc .done
    cmp eax, 0x534D4150
    jne .done
    test ecx, ecx
    jz .skip
    inc bp
    add di, 24
    cmp bp, E820_MAX
    jae .done
.skip:
    test ebx, ebx
    jz .done
    jmp .loop
.done:
    mov [e820_count], bp
    ret

; set_vbe busca y activa un modo 800x600x32 con framebuffer lineal y lo guarda en fb_*
set_vbe:
    mov ax, 0x4F00
    mov di, VBE_CTRL_ADDR
    int 0x10
    FIXDS
    cmp ax, 0x004F
    jne .fail
        cmp byte [VBE_CTRL_ADDR+0], 'V'
    jne .fail
    cmp byte [VBE_CTRL_ADDR+1], 'E'
    jne .fail
    cmp byte [VBE_CTRL_ADDR+2], 'S'
    jne .fail
    cmp byte [VBE_CTRL_ADDR+3], 'A'
    jne .fail

    ; VideoModePtr (far pointer) en 0x0E:0x10
    mov si, [VBE_CTRL_ADDR+0x0E]
    mov ax, [VBE_CTRL_ADDR+0x10]
    mov [vbe_off], si
    mov [vbe_seg], ax
.mode_loop:
    mov ax, [vbe_seg]
    mov fs, ax
    mov si, [vbe_off]
    mov cx, [fs:si]
    add si, 2
    mov [vbe_off], si
    cmp cx, 0xFFFF
    je .fail
    mov [vbe_mode], cx

    mov ax, 0x4F01
    mov di, VBE_MODE_ADDR
    int 0x10
    FIXDS
    cmp ax, 0x004F
    jne .mode_loop

    mov ax, [VBE_MODE_ADDR+0x00] ; atributos: bit 7 = LFB
    test ax, 0x0080
    jz .mode_loop
    cmp word [VBE_MODE_ADDR+0x12], 800
    jne .mode_loop
    cmp word [VBE_MODE_ADDR+0x14], 600
    jne .mode_loop
    cmp byte [VBE_MODE_ADDR+0x19], 32
    jne .mode_loop

    mov bx, [vbe_mode]
    or bx, 0x4000 ; activar framebuffer lineal
    mov ax, 0x4F02
    int 0x10
    FIXDS
    cmp ax, 0x004F
    jne .fail

    mov eax, [VBE_MODE_ADDR+0x28] ; PhysBasePtr
    mov [fb_addr], eax
    mov ax, [VBE_MODE_ADDR+0x10] ; BytesPerScanLine
    mov [fb_pitch], ax
    mov ax, [VBE_MODE_ADDR+0x12] ; XResolution
    mov [fb_width], ax
    mov ax, [VBE_MODE_ADDR+0x14] ; YResolution
    mov [fb_height], ax
    mov al, [VBE_MODE_ADDR+0x19] ; BitsPerPixel
    mov [fb_bpp], al
    ret
.fail:
    mov si, msg_no_vbe
    call panic

; encuentra /boot/kernel.elf en el ISO9660 y lo lee a KERNEL_STAGE_ADDR
load_kernel:
    ; PVD en el bloque logico 16.
    mov eax, 16
    mov bx, SECTOR_BUF
    call read_block
    cmp byte [SECTOR_BUF], 1
    jne .bad
    cmp byte [SECTOR_BUF+1], 'C'
    jne .bad
    cmp byte [SECTOR_BUF+2], 'D'
    jne .bad
    cmp byte [SECTOR_BUF+3], '0'
    jne .bad
    cmp byte [SECTOR_BUF+4], '0'
    jne .bad
    cmp byte [SECTOR_BUF+5], '1'
    jne .bad

    mov eax, [SECTOR_BUF+158] ; root dir extent (156+2)
    mov [root_lba], eax
    mov eax, [SECTOR_BUF+166] ; root dir size (156+10)
    mov [root_size], eax

    ; Buscar el directorio BOOT.
    mov eax, [root_lba]
    mov [dir_lba], eax
    mov eax, [root_size]
    mov [dir_size], eax
    mov si, name_boot
    call find_entry
    jc .bad
    mov eax, [found_lba]
    mov [boot_lba], eax
    mov eax, [found_size]
    mov [boot_size], eax

    ; Buscar KERNEL.ELF dentro de BOOT.
    mov eax, [boot_lba]
    mov [dir_lba], eax
    mov eax, [boot_size]
    mov [dir_size], eax
    mov si, name_kernel
    call find_entry
    jc .nokernel
    mov eax, [found_lba]
    mov [file_lba], eax
    mov eax, [found_size]
    mov [file_size], eax

    cmp dword [file_size], KERNEL_STAGE_MAX
    ja .toobig

    call read_kernel_file
    ret
.bad:
    mov si, msg_no_iso
    call panic
.nokernel:
    mov si, msg_no_kernel
    call panic
.toobig:
    mov si, msg_too_big
    call panic

read_kernel_file:
    mov eax, [file_lba]
    mov [cur_lba], eax
    mov eax, [file_size]
    mov [remaining], eax
    mov dword [kptr], KERNEL_STAGE_ADDR
.loop:
    cmp dword [remaining], 0
    jbe .done
    ; segmento:offset a partir de la direccion lineal kptr
    mov eax, [kptr]
    mov bx, ax
    and bx, 0x0F
    shr eax, 4
    mov [dap_seg], ax
    mov [dap_off], bx
    mov eax, [cur_lba]
    mov [dap_lba], eax
    call do_read
    mov eax, [remaining]
    cmp eax, 2048
    jbe .last
    add dword [kptr], 2048
    sub dword [remaining], 2048
    jmp .next
.last:
    add dword [kptr], eax
    mov dword [remaining], 0
.next:
    inc dword [cur_lba]
    jmp .loop
.done:
    ret



; recorre directorio en dir_lb/dir_size buscando el nombre apuntado por SI (que es byte de longitud + caracteres)
; deja el extent en found_lba y el tamaño en found_size
; da CF=1 si no lo encuentra
find_entry:
    mov [target_ptr], si
    mov eax, [dir_lba]
    mov [cur_lba], eax
    mov eax, [dir_size]
    mov [remaining], eax
.sec:
    cmp dword [remaining], 0
    jbe .notfound
    mov eax, [cur_lba]
    mov bx, DIR_BUF
    call read_block
    mov word [rec_off], 0
.rec:
    mov bx, [rec_off]
    cmp bx, 2048
    jae .next_sec
    movzx ax, byte [bx + DIR_BUF] ; longitud del registro
    test ax, ax
    jz .next_sec
    movzx dx, byte [bx + DIR_BUF + 32] ; longitud del nombre
    mov di, [target_ptr]
    movzx cx, byte [di]
    cmp dx, cx
    jb .advance
    ; comparar cx bytes (sin distinguir mayusculas)
    mov si, bx
    add si, DIR_BUF + 33
    mov di, [target_ptr]
    inc di
    call str_ci_eq
    test al, al
    jz .advance
    ; si el nombre es mas largo, exigir ';' (version ISO9660)
    mov bx, [rec_off]
    movzx dx, byte [bx + DIR_BUF + 32]
    mov di, [target_ptr]
    movzx cx, byte [di]
    cmp dx, cx
    je .found
    mov si, bx
    add si, DIR_BUF + 33
    add si, cx
    cmp byte [si], ';'
    jne .advance
.found:
    mov bx, [rec_off]
    mov eax, [bx + DIR_BUF + 2] ; extent LBA
    mov [found_lba], eax
    mov eax, [bx + DIR_BUF + 10] ; tamano
    mov [found_size], eax
    clc
    ret
.advance:
    mov bx, [rec_off]
    movzx ax, byte [bx + DIR_BUF]
    add [rec_off], ax
    jmp .rec
.next_sec:
    inc dword [cur_lba]
    cmp dword [remaining], 2048
    jbe .notfound
    sub dword [remaining], 2048
    jmp .sec
.notfound:
    stc
    ret

; compara CX bytes de DS:SI y DS:DI sin distinguir mayusculas
; da AL=1 si son identicos
str_ci_eq:
    push bx
    push cx
    push si
    push di
.loop:
    test cx, cx
    jz .eq
    mov al, [si]
    mov bl, [di]
    cmp al, 'a'
    jb .a_done
    cmp al, 'z'
    ja .a_done
    sub al, 0x20
.a_done:
    cmp bl, 'a'
    jb .b_done
    cmp bl, 'z'
    ja .b_done
    sub bl, 0x20
.b_done:
    cmp al, bl
    jne .neq
    inc si
    inc di
    dec cx
    jmp .loop
.eq:
    pop di
    pop si
    pop cx
    pop bx
    mov al, 1
    ret
.neq:
    pop di
    pop si
    pop cx
    pop bx
    xor al, al
    ret

; lee 2048 bytes en EAX=LBA a DS:BX
read_block:
    mov [dap_lba], eax
    mov [dap_off], bx
    mov word [dap_seg], 0
    jmp do_read

; ejecuta la lectura EDD con el DAP actual
do_read:
    mov dl, [boot_drive]
    mov si, dap
    mov ah, 0x42
    int 0x13
    FIXDS
    jc .err
    ret
.err:
    mov si, msg_io
    call panic

; Ahora si lo bueno bueno
; carga la GDT, enciende PE y salta a 32 bits
enter_pm:
    cli
    lgdt [gdt_ptr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm_entry

BITS 32
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, PM_STACK_TOP
    cld

    call load_elf
    call build_bootinfo

    mov eax, BOOT_MAGIC
    mov ebx, BOOT_INFO_ADDR
    mov ecx, [kernel_entry_phys]
    jmp ecx

hang32:
    cli
    hlt
    jmp hang32

; copia segmentos de PT_LOAD del ELF(staging) a su p_paddr y pone a cero la parte de .bss
load_elf:
    mov esi, KERNEL_STAGE_ADDR
    cmp dword [esi], 0x464C457F ; 0x7F 'E' 'L' 'F'
    jne .bad
    movzx eax, word [esi+42] ; e_phentsize
    mov [phentsize], eax
    movzx eax, word [esi+44] ; e_phnum
    mov [phnum], eax
    mov eax, [esi+28] ; e_phoff
    add eax, KERNEL_STAGE_ADDR
    mov [phdr_ptr], eax
    mov eax, [esi+24] ; e_entry
    mov [kernel_entry_phys], eax
    mov dword [kernel_phys_val], 0x00100000
    xor ebx, ebx
.ph:
    cmp ebx, [phnum]
    jae .done
    mov eax, [phentsize]
    imul eax, ebx
    add eax, [phdr_ptr]
    mov edi, eax
    cmp dword [edi], 1 ; PT_LOAD
    jne .next
    mov ecx, [edi+16] ; p_filesz
    mov esi, [edi+4] ; p_offset
    add esi, KERNEL_STAGE_ADDR
    mov edx, [edi+12] ; p_paddr
    mov [filesz_tmp], ecx
    cmp edx, [kernel_phys_val]
    jae .nomin
    mov [kernel_phys_val], edx
.nomin:
    mov edi, edx
    rep movsb
    ; limpiar .bss (p_memsz - p_filesz)
    mov eax, [phentsize]
    imul eax, ebx
    add eax, [phdr_ptr]
    mov edi, eax
    mov ecx, [edi+20] ; p_memsz
    sub ecx, [edi+16] ; - p_filesz
    jbe .next
    mov edi, [edi+12] ; p_paddr
    add edi, [filesz_tmp]
    xor eax, eax
    rep stosb
.next:
    inc ebx
    jmp .ph
.done:
    ret
.bad:
    jmp hang32

build_bootinfo:
    mov edi, BOOT_INFO_ADDR
    mov dword [edi+0], BOOT_MAGIC
    mov dword [edi+4], 1

    ; bootloader[16] = "tinkerbell"
    mov dword [edi+8],  0x6B6E6974  ; " tink" -> bytes "tink"
    mov dword [edi+12], 0x6C656272  ; "rbel" -> bytes "rbel"
    mov word  [edi+16], 0x006C     ; "l\0"
    mov dword [edi+20], 0

    movzx eax, byte [boot_drive]
    mov [edi+24], eax
    movzx eax, word [e820_count]
    mov [edi+28], eax
    mov dword [edi+32], E820_ADDR
    mov eax, [fb_addr]
    mov [edi+36], eax
    mov dword [edi+40], 0
    movzx eax, word [fb_pitch]
    mov [edi+44], eax
    movzx eax, word [fb_width]
    mov [edi+48], eax
    movzx eax, word [fb_height]
    mov [edi+52], eax
    movzx eax, byte [fb_bpp]
    mov [edi+56], eax
    mov dword [edi+60], 1
    mov eax, [kernel_phys_val]
    mov [edi+64], eax
    mov eax, [kernel_entry_phys]
    mov [edi+68], eax
    ret

; datos
align 8
gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF ; codigo kernel, base 0, limite 4 GiB
    dq 0x00CF92000000FFFF ; datos kernel
gdt_end:
gdt_ptr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

align 4
dap:
    db 0x10
    db 0
    dw 1 ; 1 bloque (2048 bytes en CD)
dap_off:
    dw 0
dap_seg:
    dw 0
dap_lba:
    dq 0

; Variables (inicializadas para que formen parte de la imagen)
boot_drive:  db 0
e820_count: dw 0
vbe_off: dw 0
vbe_seg: dw 0
vbe_mode: dw 0
fb_addr: dd 0
fb_pitch: dw 0
fb_width: dw 0
fb_height: dw 0
fb_bpp:  db 0
root_lba: dd 0
root_size: dd 0
boot_lba: dd 0
boot_size: dd 0
file_lba: dd 0
file_size: dd 0
dir_lba: dd 0
dir_size: dd 0
cur_lba: dd 0
remaining: dd 0
rec_off: dw 0
target_ptr: dw 0
found_lba: dd 0
found_size: dd 0
kptr: dd 0
kernel_entry_phys: dd 0
kernel_phys_val: dd 0
phentsize: dd 0
phnum: dd 0
phdr_ptr: dd 0
filesz_tmp: dd 0

; Nombres a buscar (byte de longitud + caracteres).
name_boot:        db 4, "BOOT"
name_kernel:      db 10, "KERNEL.ELF"

msg_banner: db "tinkerbell: stage2", 13, 10, 0
msg_no_edd: db "tinkerbell: INT 13h EDD is not available", 13, 10, 0
msg_no_vbe: db "tinkerbell: there is no 800x600x32 VBE mode", 13, 10, 0
msg_io: db "tinkerbell: disk error", 13, 10, 0
msg_no_iso: db "tinkerbell: ISO9660 is not valid, or kernel.elf is not there", 13, 10, 0
msg_no_kernel: db "tinkerbell: /boot/kernel.elf was not found", 13, 10, 0
msg_too_big: db "tinkerbell: kernel.elf is very big", 13, 10, 0
