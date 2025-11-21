#include <efi.h>
#include "systab.h"
#include "memory.h"
#include "console.h"
#include <inttypes.h>
#include <string.h>


#ifdef DEBUG
static void print_uefi_info(void)
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

    efi_puts("\n");
}
#endif


static void __unused disable_watchdog_timer(void)
{
    BS->set_watchdog_timer(0, 0, 0, NULL);
}


static void __noreturn efi_exit(efi_handle_t handle, efi_status_t status)
{
#ifdef DEBUG
    efi_puts("Exiting with status code ");
    efi_put0h(status);
    efi_puts("\n");
    BS->stall(10 * 1000000);
#endif
    BS->exit(handle, status, 0, NULL);

    for (;;) {
        asm("hlt");
    }
}


/*
 * EFI image entry point
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#uefi-image-entry-point
 */
efi_status_t __efiapi __noreturn uefistub_pe_entry(efi_handle_t imghandle, struct efi_system_table *systab)
{
    efi_status_t status;
    ST = systab;
    BS = (const struct efi_boot_services*) ST->boot_services;;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        efi_exit(imghandle, EFI_INVALID_PARAMETER);
    }

    // it was necessary to reset the console
    // on real hardware, even though it makes
    // qemu really slow
    efi_console_reset();

#ifdef DEBUG
    disable_watchdog_timer();
    efi_console_clear_screen();
    print_uefi_info();
#endif

    // FIXME: allocate boot_params using allocate_memory_pages so we can pass it on
    struct screen_info si;
    memset(&si, 0, sizeof(si));

    status = efi_setup_gop(&si, GRAPHICS_MODE_HIGHEST_RESOLUTION);
    if (status != EFI_SUCCESS) {
        efi_puts("ERROR: Unable to set up graphics\n");
        efi_exit(imghandle, status);
    }

#ifdef DEBUG
    efi_console_clear_screen();
#endif

    for (uint16_t y = 0; y < si.lfb_height; ++y) {
        // much quicker to access x axis in inner loop
        for (uint16_t x = 0; x < si.lfb_width; ++x) {
            uint32_t pixel = \
                (0x91 << si.red_pos) | (0x3f << si.green_pos) | (0x92 << si.blue_pos);
            *((uint32_t*) (si.lfb_base + 4 * si.lfb_linelength * y + 4 * x)) = pixel;
        }
    }

    efi_puts("Resolution: ");
    efi_putd(si.lfb_width);
    efi_puts("x");
    efi_putd(si.lfb_height);
    efi_puts(":");
    efi_putd(si.lfb_depth);
    efi_puts("\n");
    efi_puts("R:");
    efi_putd(si.red_size);
    efi_puts("-");
    efi_putd(si.red_pos);
    efi_puts(" ");
    efi_puts("G:");
    efi_putd(si.green_size);
    efi_puts("-");
    efi_putd(si.green_pos);
    efi_puts(" ");
    efi_puts("B:");
    efi_putd(si.blue_size);
    efi_puts("-");
    efi_putd(si.blue_pos);
    efi_puts("\n");

    // call ExitBootServices in future
    efi_puts("Goodbye!\n");
    for (;;) {
        BS->stall(2 * 1000000);
    }
}
