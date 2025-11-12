PROJECT := ibsenos

OVMF_PATH ?= /usr/share/OVMF/OVMF_CODE.fd 

ARCH_TARGET ?= x86_64
ARCH_HOSTED ?= w64

BUILD_DIR ?= build

CC := $(ARCH_TARGET)-$(ARCH_HOSTED)-mingw32-gcc
LD := $(CC)
AS := $(CC)

WARNINGS := all extra shadow unused error-implicit-function-declaration
CFLAGS := -std=gnu99 -ffreestanding -nostartfiles -nostdlib -fno-stack-protector -fpic -fshort-wchar -mno-red-zone

ifneq ($(BUILD_RELEASE),)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -DNDEBUG -O2
endif

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
	$$(CC) $$(CFLAGS) $$(addprefix -W,$(WARNINGS)) -c -o $$@ $$<
endef


.PHONY: all clean image iso qemu-graphic qemu-nographic qemu-iso

all: iso

# Boot loader target
$(eval $(call target,bootloader,BOOTX64.EFI,efi_main,bootloader/bootloader.o))

clean: $(TARGETS:%=%-clean)
	$(RM) $(BUILD_DIR)/$(PROJECT).iso $(BUILD_DIR)/$(PROJECT).img

iso: $(BUILD_DIR)/$(PROJECT).iso

image: $(BUILD_DIR)/$(PROJECT).img

qemu-iso: $(BUILD_DIR)/$(PROJECT).iso
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) -cdrom $< -nographic

qemu-nographic: $(BUILD_DIR)/$(PROJECT).img
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) -drive format=raw,file=$< -nographic

qemu-graphic: $(BUILD_DIR)/$(PROJECT).img
	qemu-system-$(ARCH_TARGET) -net none -bios $(OVMF_PATH) -drive format=raw,file=$<

$(BUILD_DIR)/$(PROJECT).iso: $(BUILD_DIR)/$(PROJECT).img
	@TEMPDIR=$$(mktemp -d); \
	trap 'rm -rf $$TEMPDIR' EXIT INT TERM; \
	cp $< $$TEMPDIR ; \
	xorriso -as mkisofs -R -f -e $(PROJECT).img $< -no-emul-boot -o $@ $$TEMPDIR 

$(BUILD_DIR)/$(PROJECT).img: $(bootloader-build)
	dd if=/dev/zero of=$@ bs=1M count=32
	mformat -i $@ ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -v -i $@ $< ::/EFI/BOOT
