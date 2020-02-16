NAME := ibsenos

CC := i686-elf-gcc
LD := i686-elf-ld
AS := i686-elf-as

CFLAGS := -std=gnu99 -Wall -Wextra -nostdlib 

KOBJS := entry.o kernel.o removeme.o

.PHONY: all clean distclean 
all: $(NAME).img

$(NAME).img: bootblock.bin kernel.bin
	cat $^ > $@

bootblock.bin: bootloader.o
	$(LD) -T bootblock.ld -o $@ $^ --oformat=binary

kernel.bin: $(KOBJS)
	$(LD) -T kernel.ld -o $@ $^ --oformat=binary

clean:
	$(RM) $(KOBJS)
	$(RM) bootloader.o
	$(RM) $(NAME).img kernel.bin bootblock.bin

%.o: %.s
	$(AS) -c -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
