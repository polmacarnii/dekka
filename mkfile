CC=i386-tcc
LD=i686-elf-ld
AS=nasm
CFLAGS=-ffreestanding -nostdlib -nostdinc -fno-builtin
CPPFLAGS=-Ikernel/include -Ikernel/arch/x86 -Ikernel/multitask -Iboot/x86

OBJS = build/kernel/arch/x86/entry.o build/kernel/arch/x86/halt.o \
       build/kernel/arch/x86/carch.o build/kernel/kmain.o \
       build/kernel/arch/x86/gdt.o build/kernel/arch/x86/idt.o \
       build/kernel/arch/x86/irq.o build/kernel/arch/x86/isr.o \
       build/kernel/arch/x86/cpu.o build/kernel/arch/x86/pit.o \
       build/kernel/arch/x86/asm/cpu.o build/kernel/arch/x86/asm/irq.o \
       build/kernel/arch/x86/asm/isr.o build/kernel/arch/x86/switch.o \
       build/kernel/dev/fb/fb/fb.o build/kernel/dev/fb/fb/font.o \
       build/kernel/misc/font8x8.o build/kernel/klib/mem.o \
       build/kernel/klib/panic.o build/kernel/klib/print.o \
       build/kernel/dev/term/term.o build/kernel/dev/drivers.o \
       build/kernel/multitask/proc.o

BOOT_IMG = build/boot/boot.img

all: build/kernel.elf $BOOT_IMG
	echo "dekka: kernel + bootloader"

kernel: build/kernel.elf
	echo "dekka: kernel"

boot: $BOOT_IMG
	echo "dekka: bootloader"

build/kernel.elf: $OBJS kernel/arch/x86/kernel.ld
	mkdir -p build
	$LD -T kernel/arch/x86/kernel.ld -o $target $OBJS

build/%.o: %.c
	mkdir -p $(dirname $target)
	$CC $CFLAGS $CPPFLAGS -c $stem.c -o $target

build/%.o: %.asm
	mkdir -p $(dirname $target)
	$AS -f elf32 $stem.asm -o $target

build/boot/stage1.bin: boot/x86/stage1.asm
	mkdir -p build/boot
	$AS -f bin -o build/boot/stage1.bin boot/x86/stage1.asm

build/boot/stage2.bin: boot/x86/stage2.asm boot/x86/bootinfo.h
	mkdir -p build/boot
	$AS -f bin -o build/boot/stage2.bin boot/x86/stage2.asm

$BOOT_IMG: build/boot/stage1.bin build/boot/stage2.bin
	mkdir -p build/boot
	cat build/boot/stage1.bin build/boot/stage2.bin >build/boot/boot.img
	dd if=/dev/zero of=build/boot/boot.img bs=512 count=1 seek=63 conv=notrunc 2>/dev/null

iso: build/dekka.iso
	echo "dekka: build/dekka.iso"

build/dekka.iso: build/kernel.elf $BOOT_IMG
	rm -rf build/iso
	mkdir -p build/iso/boot
	cp build/kernel.elf build/iso/boot/kernel.elf
	cp $BOOT_IMG build/iso/boot/boot.img
	xorriso -as mkisofs -o build/dekka.iso -b boot/boot.img -no-emul-boot -boot-load-size 64 build/iso

lsp:
	rm -f .clangd
	printf 'CompileFlags:\n  Add:\n' > .clangd
	printf '    - -ffreestanding\n    - -nostdlib\n    - -nostdinc\n    - -fno-builtin\n' >> .clangd
	printf '    - -I'$PWD'/kernel/include\n' >> .clangd
	printf '    - -I'$PWD'/kernel/arch/x86\n' >> .clangd
	printf '    - -I'$PWD'/kernel/multitask\n' >> .clangd
	printf '    - -I'$PWD'/boot/x86\n' >> .clangd
	printf '    - --target=i386-none-elf\n' >> .clangd

clean:
	rm -rf build .clangd

