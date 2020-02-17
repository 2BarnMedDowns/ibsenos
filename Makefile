PROJECT := ibsenos

EFI_INCLUDE := /usr/include/efi
EFI_LIBRARY := /usr/lib
OVMF_PATH := /usr/share/OVMF/OVMF_CODE.fd 

ARCH_TARGET := x86_64
ARCH_HOSTED := w64

CC := $(ARCH_TARGET)-$(ARCH_HOSTED)-mingw32-gcc
LD := $(CC)
AS := $(CC)

WARNINGS = all extra shadow unused error-implicit-function-declaration

WARNS := $(addprefix -W,$(WARNINGS))
CFLAGS := -std=gnu99 -ffreestanding -nostartfiles -nostdlib -fno-stack-protector -fpic -fshort-wchar -mno-red-zone
CFLAGS += -I$(EFI_INCLUDE) -I$(EFI_INCLUDE)/$(ARCH_TARGET) -I$(EFI_INCLUDE)/protocol
LDFLAGS := -L$(EFI_LIBRARY) -lefi -lgnuefi -lgcc -e efi_main -Wl,-dll -shared -Wl,--subsystem,10 

.PHONY: $(PROJECT).img loader image iso all clean distclean qemu-graphic qemu-nographic qemu-iso
all: iso

clean:
	$(RM) loader.o
	$(RM) $(PROJECT).img BOOTX64.EFI

iso: $(PROJECT).iso

image: $(PROJECT).img

qemu-iso: $(PROJECT).iso
	qemu-system-x86_64 -net none -bios $(OVMF_PATH) -cdrom $< -nographic

qemu-nographic: $(PROJECT).img
	qemu-system-x86_64 -net none -bios $(OVMF_PATH) -drive format=raw,file=$< -nographic

qemu-graphic: $(PROJECT).img
	qemu-system-x86_64 -net none -bios $(OVMF_PATH) -drive format=raw,file=$<

$(PROJECT).iso: $(PROJECT).img
	mkdir -p /tmp/iso
	cp $< /tmp/iso/
	xorriso -as mkisofs -R -f -e $< -no-emul-boot -o $@ /tmp/iso

$(PROJECT).img: BOOTX64.EFI
	dd if=/dev/zero of=$@ bs=1M count=32
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $< ::/EFI/BOOT

BOOTX64.EFI: loader.o
	$(LD) $(LDFLAGS) -o $@ $< 

%.o: %.c
	$(CC) $(CFLAGS) $(WARNS) -c -o $@ $<
