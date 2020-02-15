CC := i686-elf-gcc
LD := i686-elf-ld
AS := i686-elf-as

CFLAGS := -std=gnu99 -Wall -Wextra -nostdlib 
LDFLAGS := -ffreestanding -lgcc -nostdlib -std=gnu99


.PHONY: all clean distclean
all: bootloader.bin

bootloader.bin: bootloader.o
	$(CC) -T linker.ld $(LDFLAGS) -o $@ $^ 

iso:
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/cfg
	cp bootloader.bin isodir/boot/ibsenos.bin
	grub-mkrescue -o ibsenos.iso isodir

clean:
	$(RM) bootloader.o bootloader.bin

distclean:
	$(RM) -r isodir ibsenos.iso

%.o: %.s
	$(AS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
