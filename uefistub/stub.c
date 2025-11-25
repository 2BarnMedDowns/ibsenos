#include <efi.h>
#include "systab.h"
#include "memory.h"
#include "console.h"
#include <inttypes.h>
#include <string.h>
#include "pci.h"
#include <align.h>


static void __unused print_uefi_info(void)
{
    uint32_t uefirev = ST->hdr.revision;
    uint32_t uefi_major = (uefirev >> 16);
    uint32_t uefi_minor = (uefirev & 0xffff) / 10;
    uint32_t uefi_tertiary = uefirev % 10;

    uint32_t fwrev_major = (ST->fw_revision >> 16);
    uint32_t fwrev_minor = (ST->fw_revision & 0xffff);

    efi_puts("UEFI revision: ");

    efi_console_color(EFI_CONSOLE_BRIGHTCYAN);
    efi_putd(uefi_major);
    efi_puts(".");
    efi_putd(uefi_minor);
    efi_puts(".");
    efi_putd(uefi_tertiary);
    efi_console_restore();
    efi_puts("\n");

    efi_puts("Firmware vendor: ");
    efi_console_color(EFI_CONSOLE_BRIGHTMAGENTA);
    efi_char16_puts((const uint16_t*) ST->fw_vendor);
    efi_console_restore();
    efi_puts("\n");

    efi_puts("Firmware revision: ");
    efi_console_color(EFI_CONSOLE_YELLOW);
    efi_putd(fwrev_major);
    efi_puts(".");
    efi_putd(fwrev_minor);
    efi_console_restore();
    efi_puts("\n");
}


static void __unused disable_watchdog_timer(void)
{
    BS->set_watchdog_timer(0, 0, 0, NULL);
}


static void __noreturn efi_exit(efi_handle_t handle, efi_status_t status)
{
#ifdef DEBUG
    efi_puts("Exiting with status code ");
    efi_put0h(status, 16);
    efi_puts("\n");
    BS->stall(10 * 1000000);
#endif
    BS->exit(handle, status, 0, NULL);

    // Exit should not return, but
    // if it does we're in trouble
    for (;;) {
        asm("hlt");  // and catch fire
    }
}


static void efi_pci_cfg_write32(const struct efi_pci_io_protocol *pci, uint16_t offset, uint32_t value)
{
    uint64_t write_value = value;
    pci->pci_write(pci, EFI_PCI_IO_WIDTH_UINT32, offset, 1, &write_value);
}


static uint32_t efi_pci_cfg_read32(const struct efi_pci_io_protocol *pci, uint16_t offset)
{
    uint64_t read_value = ~0;
    pci->pci_read(pci, EFI_PCI_IO_WIDTH_UINT32, offset, 1, &read_value);
    return read_value;
}


//static bool efi_pci_cfg_read_base(const struct efi_pci_io_protocol *pci, uint8_t bar, 
//                                  uint64_t *base_addr, uint64_t *size)
//{
//    uint32_t orig32 = 0;
//    uint64_t orig64;
//    uint32_t s32 = 0;
//    uint64_t s64;
//    uint32_t mask32 = ~0;
//    uint64_t mask64;
//
//    orig32 = efi_pci_cfg_read32(pci, PCI_BASE_ADDRESS_0 + 4 * bar);
//    efi_pci_cfg_write32(pci, PCI_BASE_ADDRESS_0 + 4 * bar, orig32 | mask32);
//    s = efi_pci_cfg_read32(pci, PCI_BASE_ADDRESS_0 + 4 * bar);
//    efi_pci_cfg_write32(pci, PCI_BASE_ADDRESS_0 + 4 * bar, orig32);
//
//    if (!!(orig & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO) {
//        orig64 = orig32 & PCI_BASE_ADDRESS_IO_MASK;
//        s64 = s32 & PCI_BASE_ADDRESS_IO_MASK;
//        mask64 = PCI_BASE_ADDRESS_ADDRESS_IO_MASK;
//    } else {
//        orig64 = orig32 & PCI_BASE_ADDRESS_MEM_MASK;
//        s64 = s32 & PCI_BASE_ADDRESS_MEM_MASK;
//        mask64 = (uint32_t) PCI_BASE_ADDRESS_MEM_MASK;
//
//        if ((orig32 & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64) {
//            orig32 = efi_pci_cfg_read(pci, PCI_BASE_ADDRESS_0 + 4 * bar + 4);
//        }
//}


static void print_size(uint64_t size)
{
    if (!ALIGNED(size, 1024)) {
        efi_putd(size);
        efi_puts("B");
        return;
    }

    char *suffixes[] = {"B", "kB", "MB", "GB", "TB"};

    int i = 0;
    while (size > 1024) {
        size >>= 10;
        ++i;
        if (i == 4) {
            break;
        }
    }
    efi_putd(size);
    efi_puts(suffixes[i]);
}


static efi_status_t print_device_info(struct efi_pci_io_protocol *pci)
{
    efi_status_t status;

    uint64_t segment = 0;
    uint64_t bus = 0;
    uint64_t device = 0;
    uint64_t function = 0;

    status = pci->get_location(pci, &segment, &bus, &device, &function);
    if (status != EFI_SUCCESS) {
        efi_puts("ERROR: Unable to retrieve PCI bus information\n");
        return status;
    }

    uint64_t htype = 0;
    status = pci->pci_read(pci, EFI_PCI_IO_WIDTH_UINT8, PCI_HEADER_TYPE, 1, &htype);
    if (status != EFI_SUCCESS) {
        return status;
    }

    uint64_t command = 0;
    status = pci->pci_read(pci, EFI_PCI_IO_WIDTH_UINT16, PCI_COMMAND, 1, &command);
    if (status != EFI_SUCCESS) {
        return status;
    }

    uint64_t class_revision = 0;
    status = pci->pci_read(pci, EFI_PCI_IO_WIDTH_UINT32,
            PCI_CLASS_REVISION, 1, &class_revision);
    if (status != EFI_SUCCESS) {
        efi_puts("ERROR: Unable to read PCI class from device configuration space\n");
        return EFI_INVALID_PARAMETER;
    }

    uint16_t pci_class = class_revision >> 16;
    efi_puts("Device ");
    efi_put0h(segment, 4);
    efi_puts(":");
    efi_put0h(bus, 2);
    efi_puts(":");
    efi_put0h(device, 2);
    efi_puts(".");
    efi_putd(function);
    efi_puts(" is ");

    switch (pci_class) {
        case 0x0600:
            efi_puts("host bridge");
            break;
        case 0x0601:
            efi_puts("ISA bridge");
            break;
        case 0x0680:
            efi_puts("bridge");
            break;
        case 0x0100:
            efi_puts("SCSI storage");
            break;
        case 0x0101:
            efi_puts("IDE interface");
            break;
        case 0x0105:
            efi_puts("ATA controller");
            break;
        case 0x0300:
            efi_puts("VGA controller");
            break;
        case 0x0106:
            efi_puts("SATA controller");
            break;
        case 0x0108:
            efi_puts("NVMe controller");
            break;
        case 0x0109:
            efi_puts("universal flash storage");
            break;
        case 0x0c03:
            efi_puts("USB controller");
            break;
        default:
            efi_puts("class 0x");
            efi_put0h(pci_class, 4);
            break;
    }

    efi_puts("\n");

    if ((htype & PCI_HEADER_TYPE_MASK) != PCI_HEADER_TYPE_NORMAL) {
        return EFI_SUCCESS;
    }

    for (uint8_t idx = 0; idx < 6; ++idx) {
        uint64_t newcommand = command & ~(0x7);
        uint64_t offset = PCI_BASE_ADDRESS_0 + 4 * idx;
        status = pci->pci_write(pci,
                EFI_PCI_IO_WIDTH_UINT16,
                PCI_COMMAND,
                1, &newcommand);
        if (status != EFI_SUCCESS) {
            efi_puts("ERROR: Failed to disable\n");
            continue;
        }

        uint8_t bar = idx;
        uint32_t base_addr = efi_pci_cfg_read32(pci, offset);

        if (base_addr == 0) {
            pci->pci_write(
                    pci,
                    EFI_PCI_IO_WIDTH_UINT16,
                    PCI_COMMAND,
                    1, &command);
            continue;
        }

        uint32_t size = 0;
        uint64_t full_addr = 0;
        uint64_t full_size = 0;
        bool is_64bit = false;
        bool memory = false;
        bool prefetchable = false;

        if (!!(base_addr & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO) {
            efi_pci_cfg_write32(pci, offset, ~0);
            size = efi_pci_cfg_read32(pci, offset);
            efi_pci_cfg_write32(pci, offset, base_addr);
            full_addr = base_addr & PCI_BASE_ADDRESS_IO_MASK;
            full_size = ~(size & PCI_BASE_ADDRESS_IO_MASK) + 1;
        } else {
            memory = true;
            
            efi_pci_cfg_write32(pci, offset, ~0);
            size = efi_pci_cfg_read32(pci, offset);
            prefetchable = !!(size & PCI_BASE_ADDRESS_MEM_PREFETCH);
            is_64bit = ((size & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64);
            efi_pci_cfg_write32(pci, offset, base_addr);
            full_addr = base_addr & PCI_BASE_ADDRESS_MEM_MASK;
            full_size = ~(size & PCI_BASE_ADDRESS_MEM_MASK);

            if (is_64bit) {
                base_addr = efi_pci_cfg_read32(pci, offset + 4);
                efi_pci_cfg_write32(pci, offset + 4, ~0);
                size = efi_pci_cfg_read32(pci, offset + 4);
                efi_pci_cfg_write32(pci, offset + 4, base_addr);
                full_addr |= ((uint64_t) base_addr) << 32;
                size = ~size;
                full_size |= ((uint64_t) size) << 32;
                idx++;
            } 

            if (full_size != 0) {
                full_size += 1;
            }
        }

        // need to reenable responses before printing
        // this took a looong time to debug
        pci->pci_write(
                pci,
                EFI_PCI_IO_WIDTH_UINT16,
                PCI_COMMAND,
                1, &command);

        if (full_size > 0) {
            efi_puts("    BAR");
            efi_putd(bar);
            efi_puts(": ");
            efi_puts(memory ? "Memory" : "I/O ports");
            efi_puts(" at ");
            efi_put0h(full_addr, 16);
            efi_puts(" ");
            if (memory) {
                efi_puts("(");
                efi_puts(is_64bit ? "64-bit" : "32-bit");
                if (prefetchable) {
                    efi_puts(", prefetchable");
                }
                efi_puts(")");
            }
            efi_puts(" [size ");
            print_size(full_size);
            efi_puts("]");
            efi_puts("\n");
        }
    }

    return EFI_SUCCESS;
}


static efi_status_t efi_setup_pci(struct boot_params *params)
{
    efi_handle_t *handles = NULL;
    efi_status_t status;
    uint64_t num_handles = 0;

    status = BS->locate_handle_buffer(
            EFI_LOCATE_BY_PROTOCOL,
            &EFI_PCI_IO_PROTOCOL_GUID, 
            NULL,
            &num_handles, 
            &handles);
    if (status != EFI_SUCCESS) {
        return status;
    }

    for (uint64_t i = 0; i < num_handles; ++i) {
        efi_handle_t handle = handles[i];

        struct efi_pci_io_protocol *pci = NULL;

        status = BS->handle_protocol(
                handle,
                &EFI_PCI_IO_PROTOCOL_GUID,
                (void**) &pci);
        if (status != EFI_SUCCESS) {
            continue;
        }

        print_device_info(pci);
    }

    efi_free_buffer(handles);
    return EFI_SUCCESS;
}


/*
 * EFI image entry point
 */
efi_status_t __efiapi __noreturn uefistub_pe_entry(efi_handle_t imghandle, struct efi_system_table *systab)
{
    efi_status_t status;
    ST = systab;
    BS = (const struct efi_boot_services*) ST->boot_services;;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        efi_exit(imghandle, EFI_INVALID_PARAMETER);
    }

    // it seems it is necessary to reset the console
    // on real hardware, even though it makes
    // qemu really slow
    efi_console_reset();

#ifdef DEBUG
    disable_watchdog_timer();
    efi_console_clear_screen();
#endif

    // FIXME: allocate boot_params using allocate_memory_pages so we can pass it on
    struct boot_params boot_params;
    struct screen_info *si = &boot_params.screen_info;
    memset(&boot_params, 0, sizeof(boot_params));

    status = efi_setup_gop(si, GRAPHICS_MODE_HIGHEST_RESOLUTION);
    if (status != EFI_SUCCESS) {
        efi_puts("ERROR: Unable to set up graphics\n");
        efi_exit(imghandle, status);
    }

#ifdef DEBUG
    efi_console_clear_screen();
#endif

    for (uint16_t y = 0; y < si->lfb_height; ++y) {
        // much quicker to access x axis in inner loop
        for (uint16_t x = 0; x < si->lfb_width; ++x) {
            uint32_t pixel = \
                (0x91 << si->red_pos) | (0x3f << si->green_pos) | (0x92 << si->blue_pos);
            *((uint32_t*) (si->lfb_base + si->lfb_linelength * y + 4 * x)) = pixel;
        }
    }

    print_uefi_info();
    efi_puts("Resolution: ");
    efi_putd(si->lfb_width);
    efi_puts("x");
    efi_putd(si->lfb_height);
    efi_puts(":");
    efi_putd(si->lfb_depth);
    efi_puts("\n");
    efi_puts("R:");
    efi_putd(si->red_size);
    efi_puts("-");
    efi_putd(si->red_pos);
    efi_puts(" ");
    efi_puts("G:");
    efi_putd(si->green_size);
    efi_puts("-");
    efi_putd(si->green_pos);
    efi_puts(" ");
    efi_puts("B:");
    efi_putd(si->blue_size);
    efi_puts("-");
    efi_putd(si->blue_pos);
    efi_puts("\n");
    efi_puts("Frame buffer address: 0x");
    efi_put0h(si->lfb_base, 8);
    efi_puts("\n");
    efi_puts("Frame buffer size: ");
    print_size(si->lfb_size);
    efi_puts("\n");

    efi_setup_pci(&boot_params);

    // call ExitBootServices in future
    efi_puts("Goodbye!\n");
    for (;;) {
        BS->stall(2 * 1000000);
    }
}
