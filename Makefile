NAME := ibsenos
BUILD_DIR := build

CC := i686-elf-gcc
LD := i686-elf-ld
AS := i686-elf-as

KERNEL_ADDR = 0x1000
KERNEL_STACK = 0xa0000

CFLAGS := -std=gnu99 -Wall -Wextra -nostdlib -DKERNEL_ADDR=$(KERNEL_ADDR)
ASMFLAGS := --defsym KERNEL_ADDR=$(KERNEL_ADDR) --defsym KERNEL_STACK=$(KERNEL_STACK) -R

KSRCS := entry.s kernel.c removeme.c
KOBJS := $(KSRCS:%=$(BUILD_DIR)/%.o)


.PHONY: all clean distclean 
all: $(NAME).img

$(NAME).img: $(BUILD_DIR)/bootblock.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@

$(BUILD_DIR)/bootblock.bin: $(BUILD_DIR)/bootloader.o
	$(LD) -T bootblock.ld -o $@ $^ --oformat=binary

$(BUILD_DIR)/kernel.bin: $(KOBJS)
	$(LD) -Ttext $(KERNEL_ADDR) -o $@ $^ --oformat=binary

clean:
	$(RM) $(KOBJS)
	$(RM) $(BUILD_DIR)/bootloader.o
	$(RM) $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/bootblock.bin

distclean: clean
	$(RM) $(NAME).img 
	rmdir $(BUILD_DIR)

$(BUILD_DIR)/bootloader.o: bootloader.s | $(BUILD_DIR)/kernel.bin
	$(AS) $(ASMFLAGS) -c -o $@ $< --defsym KERNEL_SIZE=$(firstword $(shell du -b $(BUILD_DIR)/kernel.bin))

$(BUILD_DIR)/%.s.o: %.s | $(BUILD_DIR)
	$(AS) $(ASMFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.c.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
