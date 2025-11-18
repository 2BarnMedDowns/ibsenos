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

    uint64_t phys_addrs[3] = {0};  // phys_addrs[0] to see that there is no memory map table when we haven't allocated anything
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

    // Get the memory map
    struct efi_memory_map_data map_info = {0};
    void *map = NULL;
    status = efi_load_memory_map(&map, &map_info);
    if (status != EFI_SUCCESS) {
        efi_puts("Failed to get memory map or there is no allocated memory\n");
        goto leave;
    }

    uint64_t num_entries = map_info.map_size / map_info.desc_size;
    efi_puts("Number of memory map entries: ");
    efi_putd(num_entries);
    efi_puts("\n\n");

    for (uint64_t idx = 0; idx < num_entries; ++idx) {
        struct efi_memory_desc *md = efi_memory_desc_ptr(map, map_info.desc_size, idx);

        if (md->type == EFI_LOADER_DATA) {
            efi_puts("Entry ");
            efi_putd(idx);
            efi_puts(" is type EfiLoaderData\n");
            efi_puts("Physical start address: ");
            efi_puth(md->phys_start);
            efi_puts("\n");
            efi_puts("Number of pages: ");
            efi_putd(md->num_pages);
            efi_puts("\n");
            efi_puts("Memory attributes: \n");
            efi_put0h(md->attribute);
            efi_puts("\n\n");
        }
    }

leave:
    // We must free all memory allocated here, according to the 7.2. part of the spec
    // if we want to move some memory, we need to install stuff into the configuration table
    efi_free_buffer(map);

    for (size_t i = 0; i < array_size(phys_addrs); ++i) {
        efi_free_memory_pages(num_pages_per_chunk * EFI_PAGE_SIZE, phys_addrs[i]);
    }
    
    efi_puts("Bye\n");

    // call ExitBootServices in future
    for (;;) {}
}
