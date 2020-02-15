NAME := ibsenos

CC := i686-elf-gcc
LD := i686-elf-ld
AS := i686-elf-as

CFLAGS := -std=gnu99 -Wall -Wextra -nostdlib 

OBJS := bootloader.o kernel.o

.PHONY: all clean distclean
all: $(NAME).img

$(NAME).img: $(OBJS)
	$(LD) -T bootblock.ld -o $@ $^ --oformat=binary

clean:
	$(RM) $(OBJS)
	$(RM) $(NAME).img 

%.o: %.s
	$(AS) -c -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
