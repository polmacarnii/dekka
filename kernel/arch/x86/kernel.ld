OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_entry)

SECTIONS{
    . = 0x100000;

    .text : {
        *(.multiboot*)
        *(.text .text.*)
    } . = ALIGN(0x1000);

    .rodata : {
        *(.rodata .rodata.*)
    } . = ALIGN(0x1000);

    .data : {
        *(.data .data.*)
    } . = ALIGN(0x1000);

    .bss :{
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.eh_frame*)
        *(.comment*)
        *(.note*)
    }
}
