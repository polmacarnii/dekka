BITS 32

section .text
global halt

halt:
    sti; por si hace falta esperar interrupciones
    hlt
    ret
