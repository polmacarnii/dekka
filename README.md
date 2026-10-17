# Dekka OS
Another try of making an os.

## Compiling
### Dependencies
- tcc with crosscompiler for i386-tcc
- nasm
- i686-elf-ld
- xorriso
- mk

### Compiling
```
mk
mk iso
```

## Running
```
qemu-system-i386 -cdrom build/dekka.iso
```

## Developing
The code is right there, if you want LSP just do
```
mk lsp
```


