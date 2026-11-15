#!/bin/sh
set -e

CC=i386-tcc
LD=i686-elf-ld
AS=nasm

CFLAGS="-ffreestanding -nostdlib -nostdinc -fno-builtin"
CPPFLAGS="-Ikernel/include -Ikernel/arch/x86 -Ikernel/multitask -Iboot/x86"

KERNELDIR=$(pwd)/build/kernel
BOOTLOADERDIR=$(pwd)/build/bootloader

bootloader() {
	# compilar el bootloader
	$AS -f bin -o $BOOTLOADERDIR/stage1.bin boot/x86/stage1.asm
	$AS -f bin -o $BOOTLOADERDIR/stage2.bin boot/x86/stage2.asm
	cat $BOOTLOADERDIR/stage1.bin $BOOTLOADERDIR/stage2.bin >$BOOTLOADERDIR/boot.img
	dd if=/dev/zero of=$BOOTLOADERDIR/boot.img bs=512 count=1 seek=63 conv=notrunc 2>/dev/null
}

kernel() {
	# compilar
	# --- asm ---
	$AS -f elf32 -o $KERNELDIR/arch/x86/entry.o kernel/arch/x86/entry.asm
	$AS -f elf32 -o $KERNELDIR/arch/x86/halt.o kernel/arch/x86/halt.asm
	$AS -f elf32 -o $KERNELDIR/arch/x86/asm/cpu.o kernel/arch/x86/asm/cpu.asm
	$AS -f elf32 -o $KERNELDIR/arch/x86/asm/irq.o kernel/arch/x86/asm/irq.asm
	$AS -f elf32 -o $KERNELDIR/arch/x86/asm/isr.o kernel/arch/x86/asm/isr.asm
	$AS -f elf32 -o $KERNELDIR/arch/x86/switch.o kernel/arch/x86/switch.asm

	# --- c ---
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/carch.c -o $KERNELDIR/arch/x86/carch.o
	$CC $CFLAGS $CPPFLAGS -c kernel/kmain.c -o $KERNELDIR/kmain.o
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/gdt.c -o $KERNELDIR/arch/x86/gdt.o
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/idt.c -o $KERNELDIR/arch/x86/idt.o
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/irq.c -o $KERNELDIR/arch/x86/irq.o
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/isr.c -o $KERNELDIR/arch/x86/isr.o
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/cpu.c -o $KERNELDIR/arch/x86/cpu.o
	$CC $CFLAGS $CPPFLAGS -c kernel/arch/x86/pit.c -o $KERNELDIR/arch/x86/pit.o
	$CC $CFLAGS $CPPFLAGS -c kernel/dev/fb/fb/fb.c -o $KERNELDIR/dev/fb/fb/fb.o
	$CC $CFLAGS $CPPFLAGS -c kernel/dev/fb/fb/font.c -o $KERNELDIR/dev/fb/fb/font.o
	$CC $CFLAGS $CPPFLAGS -c kernel/misc/font8x8.c -o $KERNELDIR/misc/font8x8.o
	$CC $CFLAGS $CPPFLAGS -c kernel/klib/mem.c -o $KERNELDIR/klib/mem.o
	$CC $CFLAGS $CPPFLAGS -c kernel/klib/panic.c -o $KERNELDIR/klib/panic.o
	$CC $CFLAGS $CPPFLAGS -c kernel/klib/print.c -o $KERNELDIR/klib/print.o
	$CC $CFLAGS $CPPFLAGS -c kernel/dev/term/term.c -o $KERNELDIR/dev/term/term.o
	$CC $CFLAGS $CPPFLAGS -c kernel/dev/drivers.c -o $KERNELDIR/dev/drivers.o
	$CC $CFLAGS $CPPFLAGS -c kernel/multitask/proc.c -o $KERNELDIR/multitask/proc.o

	# --- linkear ---
	$LD -T kernel/arch/x86/kernel.ld -o build/kernel.elf \
		$KERNELDIR/arch/x86/entry.o \
		$KERNELDIR/arch/x86/halt.o \
		$KERNELDIR/arch/x86/carch.o \
		$KERNELDIR/kmain.o \
		$KERNELDIR/arch/x86/gdt.o \
		$KERNELDIR/arch/x86/idt.o \
		$KERNELDIR/arch/x86/irq.o \
		$KERNELDIR/arch/x86/isr.o \
		$KERNELDIR/arch/x86/cpu.o \
		$KERNELDIR/arch/x86/pit.o \
		$KERNELDIR/arch/x86/asm/cpu.o \
		$KERNELDIR/arch/x86/asm/irq.o \
		$KERNELDIR/arch/x86/asm/isr.o \
		$KERNELDIR/arch/x86/switch.o \
		$KERNELDIR/dev/fb/fb/fb.o \
		$KERNELDIR/dev/fb/fb/font.o \
		$KERNELDIR/misc/font8x8.o \
		$KERNELDIR/klib/mem.o \
		$KERNELDIR/klib/panic.o \
		$KERNELDIR/klib/print.o \
		$KERNELDIR/dev/term/term.o \
		$KERNELDIR/dev/drivers.o \
		$KERNELDIR/multitask/proc.o
}

iso() {
	# crear iso
	rm -rf build/iso
	mkdir -p build/iso/boot
	cp build/kernel.elf build/iso/boot/kernel.elf
	cp $BOOTLOADERDIR/boot.img build/iso/boot/boot.img
	xorriso -as mkisofs -o build/dekka.iso -b boot/boot.img -no-emul-boot -boot-load-size 64 build/iso
}

mkdir -p $KERNELDIR/arch/x86/asm \
	$KERNELDIR/dev/fb/fb \
	$KERNELDIR/dev/term \
	$KERNELDIR/misc \
	$KERNELDIR/klib \
	$KERNELDIR/multitask \
	$BOOTLOADERDIR

bootloader
kernel
iso
