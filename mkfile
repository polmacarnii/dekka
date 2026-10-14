CC=i386-tcc
LD=i686-elf-ld
AS=nasm
CFLAGS=-ffreestanding -nostdlib -nostdinc -fno-builtin
CPPFLAGS=-Ikernel/include -Ikernel/arch/x86

OBJS = build/kernel/arch/x86/entry.o build/kernel/arch/x86/halt.o \
       build/kernel/arch/x86/carch.o build/kernel/kmain.o \
       build/kernel/arch/x86/gdt.o build/kernel/arch/x86/idt.o \
       build/kernel/arch/x86/irq.o build/kernel/arch/x86/isr.o \
       build/kernel/arch/x86/cpu.o \
       build/kernel/arch/x86/asm/cpu.o build/kernel/arch/x86/asm/irq.o \
       build/kernel/arch/x86/asm/isr.o \
       build/kernel/dev/fb/fb/fb.o build/kernel/dev/fb/fb/font.o \
       build/kernel/misc/font8x8.o build/kernel/klib/mem.o \
       build/kernel/klib/panic.o build/kernel/klib/print.o \
       build/kernel/dev/term/term.o build/kernel/dev/drivers.o

build/kernel.elf: $OBJS kernel/arch/x86/kernel.ld
	mkdir -p build
	$LD -T kernel/arch/x86/kernel.ld -o $target $OBJS

build/%.o: %.c
	mkdir -p $(dirname $target)
	$CC $CFLAGS $CPPFLAGS -c $stem.c -o $target

build/%.o: %.asm
	mkdir -p $(dirname $target)
	$AS -f elf32 $stem.asm -o $target

iso: build/kernel.elf grub/grub.cfg
	rm -rf build/iso
	mkdir -p build/iso/boot/grub
	cp build/kernel.elf build/iso/boot/kernel.bin
	cp grub/grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/dekka.iso build/iso

lsp:
	rm -f .clangd
	printf 'CompileFlags:\n  Add:\n' > .clangd
	printf '    - -ffreestanding\n    - -nostdlib\n    - -nostdinc\n    - -fno-builtin\n' >> .clangd
	printf '    - -I'$PWD'/kernel/include\n' >> .clangd
	printf '    - -I'$PWD'/kernel/arch/x86\n' >> .clangd
	printf '    - --target=i386-none-elf\n' >> .clangd

clean:
	rm -rf build .clangd
