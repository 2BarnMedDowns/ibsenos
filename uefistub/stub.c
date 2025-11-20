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
    ST = NULL;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        efi_exit(imghandle, EFI_INVALID_PARAMETER);
    }

    ST = systab;
    BS = (const struct efi_boot_services*) ST->boot_services;;

    efi_console_reset();
    efi_console_clear_screen();

#ifdef DEBUG
    print_uefi_info();
    disable_watchdog_timer();
#endif

    efi_puts("Press any key to continue or wait 10 seconds\n");
    struct efi_input_key key = {0};

    status = efi_wait_for_key(10 * 1000000, &key);
    if (status == EFI_TIMEOUT) {
        efi_puts("You waited 10 seconds\n");
    } else {
        uint16_t str[2];
        str[0] = key.unicode_char;
        str[1] = 0;

        efi_puts("You pressed: '");
        efi_char16_puts(str);
        efi_puts("'\n");
        efi_puts("Scan code: ");
        efi_put0h(key.scan_code);
        efi_puts("\nUnicode char: ");
        efi_put0h(key.unicode_char);
        efi_puts("\n");
    }

    efi_puts("Bye\n");


    // GOP from osdev wiki
    // Copy pasted directly, only fitted to our uefi

    efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    struct efi_graphics_output_protocol *gop;

    status = BS->locate_protocol(&gopGuid, NULL, (void**)&gop);
    if(EFI_SUCCESS != status)
    {
        efi_puts("Unable to locate GOP \n");
    }

    struct efi_graphics_output_mode_information *info;
    uint64_t size_of_info, num_modes, native_mode;

    status = gop->query_mode(gop, gop->mode==NULL?0:gop->mode->mode, &size_of_info, &info);
    // this is needed to get the current video mode
    if (status == EFI_NOT_STARTED)
    {
        status = gop->set_mode(gop, 0);
    }

    if(EFI_SUCCESS != status)
    {
        efi_puts("Unable to get native mode \n");
    }
    else
    {
        native_mode = gop->mode->mode;
        num_modes = gop->mode->max_mode;
    }

    for (size_t i = 0; i < num_modes; i++) {
        status = gop->query_mode(gop, i, &size_of_info, &info);

        efi_puts("Mode: ");
        efi_putd(i);
        efi_puts("\n");

        efi_puts("\t");
        efi_puts("Resolution: ");
        efi_putd(info->vertical_resolution);
        efi_puts("x");
        efi_putd(info->horizontal_resolution);
        efi_puts(":");
        efi_putd(info->pixel_format);
        efi_puts("\n");
    }

    uint32_t mode = 2;  //XXX This is one of the available modes from list above, this should be selected somehow
    status = gop->set_mode(gop, mode);
    if(EFI_SUCCESS != status)
    {
        efi_puts("Unable to set mode");
        efi_putd(mode);
    }
    else
    {
        // get framebuffer
        /*
        PrintLn(L"Framebuffer address %x size %d, width %d height %d pixelsperline %d",
                gop->Mode->FrameBufferBase,
                gop->Mode->FrameBufferSize,
                gop->Mode->Info->HorizontalResolution,
                gop->Mode->Info->VerticalResolution,
                gop->Mode->Info->PixelsPerScanLine
        );
         */
    }

    for (int x = 0; x < 64*64; x++) {
        for (int y = 0; y < 64*64; y++) {
            //*((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
            *((uint32_t*)(gop->mode->frame_buffer_base + 4 + gop->mode->info->pixel_per_scanline * y + 4 * x)) = x * y;
        }
    }

    // call ExitBootServices in future
    for (;;) {}
}
