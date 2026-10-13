BITS 32

section .text
global halt

halt:
    cli
    hlt
    ret