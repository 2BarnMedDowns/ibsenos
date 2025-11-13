PROJECT := ibsenos

OVMF_PATH ?= /usr/share/OVMF/OVMF_CODE.fd 

ARCH_TARGET ?= x86_64
ARCH_HOSTED ?= w64

BUILD_DIR ?= build
INCLUDE_DIR ?= /usr/include
LIBRARY_DIR ?= /usr/lib  # FIXME: Not used now, but may be needed for stdc stuff (in which case we need to depend on ARCH_TARGET)

CC := $(ARCH_TARGET)-$(ARCH_HOSTED)-mingw32-gcc
LD := $(CC)
AS := $(CC)

WARNINGS := all extra shadow unused error-implicit-function-declaration
CFLAGS := -std=gnu99 -ffreestanding -nostartfiles -nostdlib -fno-stack-protector -fpic -fshort-wchar -mno-red-zone
CFLAGS += -I$(INCLUDE_DIR)
CFLAGS += $(addprefix -W,$(WARNINGS))
LDFLAGS := -Wl,-dll -shared -Wl,--subsystem,10 


# Usage: $(call target,<target name>,<build name>,<entry function>,<space separated list of source files>)
define target
$1-build = $$(BUILD_DIR)/$2
$1-srcs = $4
$1-objs = $$(addprefix $$(BUILD_DIR)/,$$(patsubst %.c,%.o,$$(filter-out %.h,$$($1-srcs))))

.PHONY: $1 $1-clean

TARGETS += $1
$1: $$($1-build)

$1-clean:
	$$(RM) $$($1-build) $$($1-objs)

$$($1-build): $$($1-objs)
	@mkdir -p $$(dir $$@)
	$$(LD) $$(LDFLAGS) -e $3 -o $$@ $$^

$$($1-objs): $$(BUILD_DIR)/%.o : %.c $$(filter %.h,$$($1-srcs))
	@mkdir -p $$(dir $$@)
	$$(CC) $$(CFLAGS) -c -o $$@ $$<
endef


.PHONY: all clean image iso 

# Make a disk image that can be booted with Qemu
all: iso

# Boot loader target
$(eval $(call target,bootloader,BOOTX64.EFI,efi_main,bootloader/bootloader.o))


clean: $(TARGETS:%=%-clean)
	$(RM) $(BUILD_DIR)/$(PROJECT).iso $(BUILD_DIR)/$(PROJECT).img


# Build a CDROM iso
iso: $(BUILD_DIR)/$(PROJECT).iso


# Build a disk image
image: $(BUILD_DIR)/$(PROJECT).img


# Create a CDROM ISO from the disk image
$(BUILD_DIR)/$(PROJECT).iso: $(BUILD_DIR)/$(PROJECT).img
	@TEMPDIR=$$(mktemp -d) \
	trap 'rm -rf $$TEMPDIR' EXIT INT TERM; \
	cp $< $$TEMPDIR/$(PROJECT).img ; \
	xorriso -as mkisofs -R -f -e $(PROJECT).img $< -no-emul-boot -o $@ $$TEMPDIR 


# Create a disk image and format it with MSDOS file system
# This is used by the EFI boot loader
$(BUILD_DIR)/$(PROJECT).img: $(bootloader-build)
	dd if=/dev/zero of=$@ bs=1M count=32
	mformat -i $@ ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -v -i $@ $< ::/EFI/BOOT


# TODO: Move Qemu launch stuff to a virsh script instead in the future
.PHONY: qemu qemu-nographic qemu-cdrom qemu-usb

# Launch Qemu instance but using the ISO CDROM image instead of the disk
qemu-cdrom: $(BUILD_DIR)/$(PROJECT).iso
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) -cdrom $<


# Launch Qemu instance using USB stick image
qemu-usb: $(BUILD_DIR)/$(PROJECT).img
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) \
		-drive if=none,format=raw,file=$<,id=stick \
		-device nec-usb-xhci,id=xhci \
		-device usb-storage,bus=xhci.0,drive=stick,removable=on 


# Short-hand for launching a Qemu instance without graphics (terminal)
# Ctrl+A+X for exit
qemu-nographic: $(BUILD_DIR)/$(PROJECT).img
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) -nographic \
		-drive format=raw,file=$<


# Short-hand for launching a Qemu instance with graphics
# Ctrl+Alt+G for releasing mouse grab
qemu: $(BUILD_DIR)/$(PROJECT).img
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) \
		-drive format=raw,file=$<

