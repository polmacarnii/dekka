#!/bin/sh
rm -fr .clangd
touch .clang

printf 'CompileFlags:\n  Add:\n' > .clangd
printf '    - -ffreestanding\n    - -nostdlib\n    - -nostdinc\n    - -fno-builtin\n' >> .clangd
printf '    - -I'$PWD'/kernel/include\n' >> .clangd
printf '    - -I'$PWD'/kernel/arch/x86\n' >> .clangd
printf '    - -I'$PWD'/kernel/multitask\n' >> .clangd
printf '    - -I'$PWD'/boot/x86\n' >> .clangd
printf '    - --target=i386-none-elf\n' >> .clangd
