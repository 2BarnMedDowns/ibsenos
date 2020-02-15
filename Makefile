CC := i686-elf-gcc
LD := i686-elf-ld
AS := i686-elf-as

CFLAGS := -std=gnu99 -Wall -Wextra -nostdlib 

.PHONY: all clean distclean
all: bootloader.img

bootloader.img: bootloader.o
	$(LD) -o $@ $^ --oformat=binary

clean:
	$(RM) bootloader.o bootloader.img

%.o: %.s
	$(AS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
