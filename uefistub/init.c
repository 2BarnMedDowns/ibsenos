#include <efi.h>
#include "efistub.h"
#include "efi_console.h"
#include <inttypes.h>
#include <string.h>


const struct efi_system_table *ST = NULL;


static void draw_box(uint64_t x, uint64_t y, uint64_t w, uint64_t h)
{
    const struct efi_simple_text_output_protocol *conout =
        (const struct efi_simple_text_output_protocol*) ST->console_out;

    uint16_t line[w];
    line[w-1] = 0;

    conout->set_cursor(conout, x, y);
    line[0] = 0x250c;
    for (uint16_t c = 1; c < w-1; ++c) {
        line[c] = 0x2500;
    }
    line[w-1] = 0x2510;
    efi_char16_puts(line);

    for (uint16_t r = y+1; r < y+h-1; ++r) {
        conout->set_cursor(conout, x, r);
        efi_char16_puts(L"\x2502");
        conout->set_cursor(conout, x+w-1, r);
        efi_char16_puts(L"\x2502");
    }

    conout->set_cursor(conout, x, y+h-1);
    line[0] = 0x2514;
    for (uint16_t c = 1; c < w-1; ++c) {
        line[c] = 0x2500;
    }
    line[w-1] = 0x2518;
    efi_char16_puts(line);
}


/*
 * EFI image entry point
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#uefi-image-entry-point
 */
efi_status_t __efiapi uefi_entry(void *, struct efi_system_table *systab)
{
    efi_status_t status;
    ST = NULL;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        return EFI_INVALID_PARAMETER;
    }

    ST = systab;
    const struct efi_simple_text_output_protocol * console_out =
        (const struct efi_simple_text_output_protocol*) systab->console_out;

    // Find console mode with the largest rows * columns
    uint64_t console_columns = 80;
    uint64_t console_rows = 25;
    uint64_t console_mode = 0;
    for (uint64_t i = 1; i  < 32; ++i) {
        uint64_t columns = 0;
        uint64_t rows = 0;

        status = console_out->query_mode(console_out, i, &columns, &rows);
        if (status == EFI_SUCCESS && rows * columns > console_rows * console_columns) {
            console_mode = i;
            console_columns = columns;
            console_rows = rows;
        }
    }

    status = console_out->set_mode(console_out, console_mode);
    if (status != EFI_SUCCESS) {
        return EFI_LOAD_ERROR;
    }

    uint32_t uefirev = systab->hdr.revision;
    uint32_t uefi_major = (uefirev >> 16);
    uint32_t uefi_minor = (uefirev & 0xffff) / 10;
    uint32_t uefi_tertiary = uefirev % 10;

    uint32_t fwrev_major = (systab->fw_revision >> 16);
    uint32_t fwrev_minor = (systab->fw_revision & 0xffff);

    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_GRAY, EFI_CONSOLE_BLACK));
    console_out->clear_screen(console_out);

    console_out->set_cursor(console_out, 0, 0);

    efi_puts("UEFI revision: ");
    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_YELLOW, EFI_CONSOLE_BLACK));
    efi_putd(uefi_major);
    efi_puts(".");
    efi_putd(uefi_minor);
    efi_puts(".");
    efi_putd(uefi_tertiary);
    efi_puts("\n");
    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_GRAY, EFI_CONSOLE_BLACK));

    efi_puts("Firmware vendor: ");
    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_MAGENTA, EFI_CONSOLE_BLACK));
    efi_char16_puts((const uint16_t*) systab->fw_vendor);
    efi_puts("\n");
    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_GRAY, EFI_CONSOLE_BLACK));

    efi_puts("Firmware revision: ");
    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_CYAN, EFI_CONSOLE_BLACK));
    efi_putd(fwrev_major);
    efi_puts(".");
    efi_putd(fwrev_minor);
    efi_puts("\n");
    console_out->set_attribute(console_out,
            EFI_CONSOLE_COLOR(EFI_CONSOLE_GRAY, EFI_CONSOLE_BLACK));

    efi_puts("\n");

    uint64_t phys_addrs[2];
    uint64_t num_pages_per_chunk = 1;

    // Allocate some memory
    for (size_t i = 0; i < array_size(phys_addrs); ++i) {
        efi_puts("Allocating memory...");

        status = efi_allocate_memory_pages(num_pages_per_chunk * EFI_PAGE_SIZE, &phys_addrs[i], 1 << 30);
        
        efi_puts(status == EFI_SUCCESS ? "SUCCESS" : "FAILED");
        efi_puts("\n");
        
        if (status == EFI_SUCCESS) {
            efi_puts("Address: ");
            efi_puth(phys_addrs[i]);
        } else {
            efi_puts("Error code: ");
            efi_puth(status);
        }
        efi_puts("\n");
        efi_puts("\n");
    }

//    // Get the memory map
//    struct efi_memory_map *mmap = NULL;
//    status = efi_get_memory_map(&mmap);
//    if (status != EFI_SUCCESS) {
//        efi_puts("Failed to get memory map\n");
//        efi_puts("Error code: ");
//        efi_puth(status);
//        efi_puts("\n");
//        goto leave;
//    }
//
//    efi_puts("Memory map has ");
//    efi_putd(mmap->map_entries);
//    efi_puts(" descriptors\n");
//
//    // FIXME: this table does not look right, sometimes print the same address
//    // Maybe some pointer stuff is off in get_memory_map ??
//    // it may seem that we might be corrupting the image, since it kinda works
//    // the first time and then becomes weird afterwards
//    // maybe we are crashing the UEFI firmware and that in turn is corrupting the image?
//    for (uint64_t i = 0; i < mmap->map_entries; ++i) {
//        const struct efi_memory_desc *md = &mmap->map[i];
//        
//        efi_puts("Descriptor ");
//        efi_putd(i);
//        efi_puts(" has physical address ");
//        efi_puth(md->phys_start);
//        efi_puts(" and is ");
//        efi_putd(md->num_pages);
//        efi_puts(" pages\n");
//        efi_puts("Attribute: ");
//        efi_puth(md->attribute);
//        efi_puts("\n");
//    }

leave:
    for (size_t i = 0; i < array_size(phys_addrs); ++i) {
        efi_free_memory_pages(num_pages_per_chunk * EFI_PAGE_SIZE, phys_addrs[i]);
    }
    
    efi_puts("Bye\n");

    for (;;) {}
}

//
//static EFI_STATUS EFIAPI PrintLn(IN EFI_SYSTEM_TABLE * SysTbl, IN CHAR16 * String)
//{
//    EFI_STATUS Status;
//
//    Status = SysTbl->ConOut->OutputString(SysTbl->ConOut, String);
//    if (EFI_ERROR(Status)) {
//        return Status;
//    }
//
//    Status = SysTbl->ConOut->OutputString(SysTbl->ConOut, L"\n\r");
//    return Status;
//}
//
//
//EFI_STATUS EFIAPI Main(EFI_HANDLE ImgHandle, EFI_SYSTEM_TABLE* SysTbl)
//{
//    InitializeLib(ImgHandle, SysTbl);
//
//    EFI_STATUS Status;
//
//    Status = SysTbl->ConOut->ClearScreen(SysTbl->ConOut);
//    if (EFI_ERROR(Status)) {
//        PrintLn(SysTbl, L"ERROR: Failed to clear the screen");
//    }
//
//    //SysTbl->ConOut->OutputString(SysTbl->ConOut, L"Please wait while preparing OS\n\r");
//
//#ifdef DEBUG
//    SysTbl->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
//#endif
//
//    EFI_TIME now;
//    Status = SysTbl->RuntimeServices->GetTime(&now, NULL);
//    if (EFI_ERROR(Status)) {
//        PrintLn(SysTbl, L"Failed to get system time, something is wrong...");
//        return Status;
//    }
//
//    while (1);
//    return EFI_SUCCESS;
//}
